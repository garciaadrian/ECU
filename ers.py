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

cef_url = 'http://opensource.spotify.com/cefbuilds/cef_binary_3.2704.1414.g185cd6c_windows64.tar.bz2'
cef_dist_name = 'cef_binary_3.2704.1414.g185cd6c_windows64.tar.bz2'

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
    sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
    
    os.environ['PATH'] += os.pathsep + os.pathsep.join([
        self_path,
        os.path.abspath(os.path.join('tools', 'build')),
        ])
    
    if not get_bin('git'):
        print('ERROR: git must be installed and on PATH.')
        sys.exit(1)
        
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
@click.option('--configuration', type=click.Choice(['Release', 'Debug'])
              , default='Release')
@click.pass_context
def build(ctx, no_premake, configuration):
    # run pREMAKE@!#!#
    result = subprocess.call([
        'msbuild',
        'build/ECU.sln',
        '/nologo',
        '/m',
        '/v:m',
        '/p:Configuration=' + configuration,
        '/p:Platform=Windows'], shell=False)

    if result != 0:
        click.echo('ERROR: build failed with one or more errors.')
        return result
    else:
        click.echo('Success!')
                    
@cli.command()
@click.pass_context
def cef(ctx):
    if not os.path.exists('libs/' + cef_dist_name[:-8]):
        click.echo('CEF lib not found. Downloading...')
        r = wget.download('http://opensource.spotify.com/cefbuilds/cef_binary_3.2704.1414.g185cd6c_windows64.tar.bz2')
        shutil.move(cef_dist_name, 'libs/' + cef_dist_name)
        
        new_file_path = 'libs/{dist}'.format(dist=cef_dist_name)[:-4]
        cef_new = open(new_file_path, 'wb')
        cef_decompressed = bz2.BZ2Decompressor()
        with open('libs/' + cef_dist_name, 'rb') as f:
            while True:
                b = f.read(8096)
                if not b:
                    break
                cef_new.write(bz2.BZ2Decompressor.decompress(cef_decompressed, b))
        cef_new.close()
        
        print('Untar cef')
        print('Opening {file}'.format(file=new_file_path))
        tar = tarfile.open(new_file_path)
        tar.extractall('libs/')
        tar.close()
        
        print('Removing {file}'.format(file=cef_dist_name))
        os.remove('libs/{file}'.format(file=cef_dist_name))
        print('Removing {file}'.format(file=cef_dist_name[:-4]))
        os.remove('libs/{file}'.format(file=cef_dist_name[:-4]))
    else:
        print("Library cef OK")
@cli.command()
@click.pass_context
def dist(ctx):
    pass

cli.add_command(cef)
cli.add_command(clean)
cli.add_command(dist)

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

def shell_call(command, throw_on_error, stdout_path):
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
if __name__ == '__main__':
    cli(obj={})
