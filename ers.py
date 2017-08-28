import click
import os
from glob import glob
import wget
import bz2
import tarfile
import shutil
import re
import subprocess
import sys

@click.group()
@click.option('--debug/--no-debug', default=False)
@click.pass_context
def cli(ctx, debug):
    click.echo('Debug mode is {0}'.format('on' if debug else 'off'))
    ctx.obj['debug'] = debug
    
    vs_version = import_vs_environment()
    if vs_version != 2015:
        print('ERROR: Visual Studio 2015 not found!')
        print('Ensure you have the VS140COMNTOOLS environment variable!')
        sys.exit(1)
    
    self_path = os.path.dirname(os.path.abspath(__file__))
    ctx.obj['self_path'] = self_path
    
    sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
    
    os.environ['PATH'] += os.pathsep + os.pathsep.join([
        self_path,
        os.path.abspath(os.path.join('tools', 'build')),
        ])
    
    if not get_bin('git'):
        print('ERROR: git must be installed and on PATH.')
        sys.exit(1)

    if not get_bin('npm'):
        print('ERROR: npm must be installed and on PATH.')
        sys.exit(1)

@cli.command()
@click.pass_context
def client(ctx):

    base_dir = ctx.obj['self_path'] + '\\frontends\\reactjs\\'
    
    os.chdir(base_dir)
    
    if not os.path.exists(base_dir + '\\node_modules\\'):
        result = subprocess.call(['npm', 'install'], shell=True)
        if result != 0:
            click.echo('ERROR: "npm install" failed with one or more errors.')
            return result
        
    result = subprocess.call(['npm', 'run', 'build'], shell=True)
    if result != 0:
        click.echo('ERROR: "npm run build" failed with one or more errors.')
        return result

    # Destination directory must not already exist
    for config in ['Debug', 'Release']:
        dest_path = ctx.obj['self_path'] + '\\build\\bin\\{0}\\reactjs'.format(config)
        
        if os.path.exists(dest_path):
            shutil.rmtree(dest_path)
    
        shutil.copytree(base_dir + 'build', dest_path)
        
@cli.command()
@click.pass_context
def clean(ctx):
    
    paths = ['build/bin/Debug', 'build/bin/Release']
    for path in paths:
        for file in glob(path + '/*.lib'):
            os.remove(file)
        for file in glob(path + '/*.pdb'):
            os.remove(file)
        for file in glob(path + '/*.exp'):
            os.remove(file)
        try:
            os.remove(path + '/ECU.exe')
        except FileNotFoundError:
            pass

@cli.command()
@click.option('--no-premake', is_flag=True)
@click.option('--configuration', '-c', type=click.Choice(['release', 'debug'])
              , default='release')
@click.pass_context
def build(ctx, no_premake, configuration):
    if not no_premake:
        shell_call([
            os.path.join('tools', 'build', 'premake5.exe'),
            'vs2015'])
        
    generate_version_h()

    start = timer()
    result = subprocess.call([
        'msbuild',
        'build/ECU.sln',
        '/nologo',
        '/m',
        '/v:m',
        '/p:Configuration=' + configuration,
        '/p:Platform=Windows'], shell=False)
    end = timer()
    elapsed = end - start

    if result != 0:
        click.echo(click.style('ERROR: build failed with one or more errors',
                               fg='white', bg='red'), err=True)
        return result
    else:
        click.echo(click.style('SUCCESS: {0} build finished in {1} seconds'
                               .format(configuration, elapsed),
                               fg='white', bg='green'))
    shell_call([
        os.path.join('tools', 'build', 'premake5.exe'),
        'export-compile-commands'])

    
@cli.command()
@click.pass_context
def lint(ctx):
    for file_ in get_src_files():
        print(file_)
        shell_call([os.path.join('tools', 'build', 'clang-format.exe'),
                    '-style=file', '-i', file_])
                    
@cli.command()
@click.option('--configuration', '-c', type=click.Choice(['release', 'debug']),
              default='release')
@click.pass_context
def dist(ctx, configuration):
    # TODO: Run ers client and ers build before distributing
    
    if not os.path.exists('dist'):
        os.makedirs('dist')
    release_directory = 'build/bin/Release/'
    release_directory_files = ['ECU.exe',
                               'ECU.pdb',
                               'vJoyInterface.dll']
    
    shutil.copy('LICENSE.txt', 'dist')
    if configuration == 'debug':
        shutil.copy('build/bin/Release/ECU.pdb', 'dist')
        
    try:
        shutil.copytree('build/bin/Release/reactjs', 'dist/reactjs')
    except FileExistsError:
        pass
        
    for resource in release_directory_files:
        shutil.copy(release_directory + resource, 'dist')
        

@cli.command()
@click.pass_context
def premake(ctx):
    shell_call([
        os.path.join('tools', 'build', 'premake5.exe'),
        'vs2015'])
    shell_call([
        os.path.join('tools', 'build', 'premake5.exe'),
        'export-compile-commands'])
    generate_version_h()

cli.add_command(clean)
cli.add_command(dist)

def get_src_files():
    for dir_, _, files in os.walk('src'):
        for file_ in files:
            if file_.endswith(('.cc', '.h')):
                filepath = dir_ + '\\' + file_
                yield filepath.replace('\\', '/')

def import_vs_environment():
    """Finds the installed Visual Studio version and imports
    interesting environment variables into os.environ.

    Returns:
    A version such as 2015 or None if no VS is found.
    """
    version = 0
    tools_path = ''
    if 'VS140COMNTOOLS' in os.environ:
        version = 2015
        tools_path = os.environ['VS140COMNTOOLS']
    else:
        click.echo('ERROR: Visual Studio 2015 is not installed!')
        sys.exit(1)
    tools_path = os.path.join(tools_path, '..\\..\\vc\\vcvarsall.bat')

    args = [tools_path, '&&', 'set']
    popen = subprocess.Popen(
        args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    variables, _ = popen.communicate()
    envvars_to_save = (
        'devenvdir',
        'include',
        'lib',
        'libpath',
        'path',
        'pathext',
        'systemroot',
        'temp',
        'tmp',
        'windowssdkdir',
    )
    for line in variables.splitlines():
        for envvar in envvars_to_save:
            if re.match(envvar + '=', line.lower().decode()):
                var, setting = line.decode().split('=', 1)
                if envvar == 'path':
                    print(sys.executable)
                    setting = os.path.dirname(sys.executable) + os.pathsep + setting
                os.environ[var.upper()] = setting
                break
    os.environ['VSVERSION'] = str(version)
    return version

def timer():
    try:
        from time import perf_counter
    except ImportError:
        from time import clock
        
    if (sys.version_info >= (3, 3)):
        return perf_counter()
    else:
        return clock()

def get_bin(bin):
    """Checks whether the given binary is present and returns the path.

    Args:
    bin: binary name (without .exe, etc).

    Returns:
    Full path to the binary or None if not found.
    """
    for path in os.environ['PATH'].split(os.pathsep):
        path = path.strip('"')
        exe_file = os.path.join(path, bin)
        if os.path.isfile(exe_file) and os.access(exe_file, os.X_OK):
            return exe_file
        exe_file = exe_file + '.exe'
        if os.path.isfile(exe_file) and os.access(exe_file, os.X_OK):
            return exe_file
    return False

def shell_call(command, throw_on_error=True, stdout_path=None):
    """Executes a shell command.

    Args:
    command: Command to execute, as a list of parameters.
    throw_on_error: Whether to throw an error or return the status code.
    stdout_path: File path to write stdout output to.
    
    Returns:
    If throw_on_error is False the status code of the call will be returned.
    """
    stdout_file = None
    if stdout_path:
        stdout_file = open(stdout_path, 'w')
    result = 0
    try:
        if throw_on_error:
            result = 1
            subprocess.check_call(command, shell=False, stdout=stdout_file)
            result = 0
        else:
            result = subprocess.call(command, shell=False, stdout=stdout_file)
    finally:
        if stdout_file:
            stdout_file.close()
    return result

def get_git_head_info():
    """Queries the current branch and commit checksum from git.

    Returns:
    (branch_name, commit, commit_short)
    If the user is not on any branch the name will be 'detached'.
    """
    p = subprocess.Popen([
        'git',
        'symbolic-ref',
        '--short',
        '-q',
        'HEAD'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (stdout, stderr) = p.communicate()
    branch_name = stdout.strip() or 'detached'
    p = subprocess.Popen([
        'git',
        'rev-parse',
        'HEAD'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (stdout, stderr) = p.communicate()
    commit = stdout.strip() or 'unknown'
    p = subprocess.Popen([
        'git',
        'rev-parse',
        '--short',
        'HEAD'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (stdout, stderr) = p.communicate()
    commit_short = stdout.strip() or 'unknown'
    return (branch_name.decode(), commit.decode(), commit_short.decode())

def generate_version_h():
    """Generates a build/version.h file that contains current git info.
    """
    (branch_name, commit, commit_short) = get_git_head_info()
    contents = '''// Autogenerated by `ers premake`.
#ifndef GENERATED_VERSION_H_
#define GENERATED_VERSION_H_
#define ECU_BUILD_BRANCH "%s"
#define ECU_BUILD_COMMIT "%s"
#define ECU_BUILD_COMMIT_SHORT "%s"
#define ECU_BUILD_DATE __DATE__
#endif  // GENERATED_VERSION_H_
''' % (branch_name, commit, commit_short)
    with open('src/version.h', 'w') as f:
        f.write(contents)
        
def git_submodule_update():
  """Runs a full recursive git submodule init and update.

  Older versions of git do not support 'update --init --recursive'. We could
  check and run it on versions that do support it and speed things up a bit.
  """
  if True:
    shell_call([
        'git',
        'submodule',
        'update',
        '--init',
        '--recursive',
        ])
  else:
    shell_call([
        'git',
        'submodule',
        'init',
        ])
    shell_call([
        'git',
        'submodule',
        'foreach',
        '--recursive',
        'git',
        'submodule',
        'init',
        ])
    shell_call([
        'git',
        'submodule',
        'update',
        '--recursive',
        ])

if __name__ == '__main__':
    cli(obj={})
