#!/usr/bin/env python

# Copyright 2015 Ben Vanik. All Rights Reserved.
# Copyright 2017 Adrian Garcia Cruz.

"""Main build script and tooling for ECU.

Run with --help or no arguments for possible commands.
"""

__author__ = 'garcia.adrian.cruz@gmail.com (Adrian Garcia Cruz)'


import argparse
import os
import re
import shutil
import string
import subprocess
import sys


self_path = os.path.dirname(os.path.abspath(__file__))


def main():
    # Add self to the root search path.
    sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))

    # Augment path to include our fancy things.
    os.environ['PATH'] += os.pathsep + os.pathsep.join([
        self_path,
        os.path.abspath(os.path.join('tools', 'build')),
    ])

    # Check 7z exists.
    if not has_bin('7z'):
        print('ERROR: 7z must be installed and on PATH.')
        sys.exit(1)
        return


    # Check git exists.
    if not has_bin('git'):
        print('ERROR: git must be installed and on PATH.')
        sys.exit(1)
        return

    # Check cmkae exists.
    if not has_bin('cmake'):
        print('ERROR: cmake must be installed and on PATH.')
        sys.exit(1)
        return

    # Check python version.
    if not sys.version_info[:2] == (2, 7):
        # TODO(benvanik): allow things to work with 3, but warn on
        # clang-format.
        print('ERROR: Python 2.7 must be installed and on PATH')
        sys.exit(1)
        return

    # Grab Visual Studio version and execute shell to set up environment.
    if sys.platform == 'win32':
        vs_version = import_vs_environment()
        if vs_version == None:
            print('ERROR: Visual Studio not found!')
            print('Please refer to the building guide:')
            print('  https://github.com/benvanik/xenia/blob/master/docs/building.md')
            sys.exit(1)
            return

    # Setup main argument parser and common arguments.
    parser = argparse.ArgumentParser(prog='ecu')

    # Grab all commands and populate the argument parser for each.
    subparsers = parser.add_subparsers(title='subcommands',
                                       dest='subcommand')
    commands = discover_commands(subparsers)

    # If the user passed no args, die nicely.
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)
        return

    # Gather any arguments that we want to pass to child processes.
    command_args = sys.argv[1:]
    pass_args = []
    try:
        pass_index = command_args.index('--')
        pass_args = command_args[pass_index + 1:]
        command_args = command_args[:pass_index]
    except:
        pass

    # Parse command name and dispatch.
    args = vars(parser.parse_args(command_args))
    command_name = args['subcommand']
    try:
        command = commands[command_name]
        return_code = command.execute(args, pass_args, os.getcwd())
    except Exception as e:
        raise
        return_code = 1
    sys.exit(return_code)


def import_vs_environment():
    """Finds the installed Visual Studio version and imports
    interesting environment variables into os.environ.

    Returns:
      A version such as 2015 or None if no VS is found.
    """
    version = 0

    candidate_path = subprocess.check_output(
        'extern/vswhere/vswhere.exe -version "[15,)" -latest -format value -property installationPath', shell=False)
    candidate_path = candidate_path.strip()

    tools_path = ''
    if candidate_path:
        tools_path = os.path.join(
            candidate_path, 'vc\\auxiliary\\build\\vcvarsall.bat')
        if os.path.isfile(tools_path) and os.access(tools_path, os.X_OK):
            version = 2017
    if version == 0 and 'VS140COMNTOOLS' in os.environ:
        version = 2015
        tools_path = os.environ['VS140COMNTOOLS']
        tools_path = os.path.join(tools_path, '..\\..\\vc\\vcvarsall.bat')
    if version == 0:
        return None

    args = [tools_path, 'x64', '&&', 'set']
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
            if re.match(envvar + '=', line.lower()):
                var, setting = line.split('=', 1)
                if envvar == 'path':
                    setting = os.path.dirname(
                        sys.executable) + os.pathsep + setting
                os.environ[var.upper()] = setting
                break

    os.environ['VSVERSION'] = str(version)
    return version


def has_bin(bin):
    """Checks whether the given binary is present.

    Args:
      bin: binary name (without .exe, etc).

    Returns:
      True if the binary exists.
    """
    bin_path = get_bin(bin)
    if not bin_path:
        return False
    return True


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
    return None


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
        'HEAD',
    ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (stdout, stderr) = p.communicate()
    branch_name = stdout.strip() or 'detached'
    p = subprocess.Popen([
        'git',
        'rev-parse',
        'HEAD',
    ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (stdout, stderr) = p.communicate()
    commit = stdout.strip() or 'unknown'
    p = subprocess.Popen([
        'git',
        'rev-parse',
        '--short',
        'HEAD',
    ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (stdout, stderr) = p.communicate()
    commit_short = stdout.strip() or 'unknown'
    return (branch_name, commit, commit_short)


def generate_version_h():
    """Generates a build/version.h file that contains current git info.
    """
    (branch_name, commit, commit_short) = get_git_head_info()
    contents = '''// Autogenerated by `xb premake`.
#ifndef GENERATED_VERSION_H_
#define GENERATED_VERSION_H_
#define XE_BUILD_BRANCH "%s"
#define XE_BUILD_COMMIT "%s"
#define XE_BUILD_COMMIT_SHORT "%s"
#define XE_BUILD_DATE __DATE__
#endif  // GENERATED_VERSION_H_
''' % (branch_name, commit, commit_short)
    with open('build/version.h', 'w') as f:
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


def get_clang_format_binary():
    """Finds a clang-format binary. Aborts if none is found.

    Returns:
      A path to the clang-format executable.
    """
    attempts = [
        'C:\\Program Files\\LLVM\\bin\\clang-format.exe',
        'C:\\Program Files (x86)\\LLVM\\bin\\clang-format.exe',
        'clang-format-3.8',
        'clang-format',
    ]
    for binary in attempts:
        if has_bin(binary):
            return binary
    print 'ERROR: clang-format is not on PATH'
    print 'LLVM is available from http://llvm.org/releases/download.html'
    print 'At least version 3.8 is required.'
    print 'See docs/style_guide.md for instructions on how to get it.'
    sys.exit(1)


def platform_luajit_compile():
    """ Compiles luajit.
    """
    luajit_dir = os.path.join(self_path, "extern", "luajit-2.0")
    
    if sys.platform == 'win32':
        ret = subprocess.call(os.path.join(luajit_dir, "src", "msvcbuild.bat"),
                              cwd=os.path.join(luajit_dir, "src"),
                              shell=False)

def platform_openssl_extract():
    """ Extracts openssl.7z
    """
    openssl_dir = os.path.join(self_path, "extern", "openssl")
    
    if sys.platform == 'win32':
        ret = subprocess.call(['7z', 'x', 'openssl-1.1.0e-vs2017.7z'],
                              cwd=openssl_dir,
                              shell=False)

def run_cmake(generator):
    """Runs cmake on the main project with given generator
    """
    ret = subprocess.call([
        'cmake',
        '-G',
        'Visual Studio 15 Win64',
        '-B' + os.path.join(self_path, 'build'),
        '-H' + self_path
    ], shell=False)
    return ret


def run_platform_cmake():
    """Runs cmake build configuration
    """
    if sys.platform == 'win32':
        vs_version_year = '2015'
        if 'VSVERSION' in os.environ:
            vs_version_year = os.environ['VSVERSION']
        if vs_version_year == '2015':
            vs_version_numeral = '14'
        elif vs_version_year == '2017':
            vs_version_numeral = '15'

        return run_cmake('Visual Studio {0} {1}'.format(
            vs_version_numeral,
            vs_version_year))


def run_premake(target_os, action):
    """Runs premake on the main project with the given format.

    Args:
      target_os: target --os to pass to premake.
      action: action to preform.
    """
    ret = subprocess.call([
        'python',
        os.path.join('tools', 'build', 'premake'),
        '--file=premake5.lua',
        '--os=%s' % (target_os),
        '--cc=clang',
        '--test-suite-mode=combined',
        '--verbose',
        action,
    ], shell=False)

    if ret == 0:
        generate_version_h()

    return ret


def run_premake_clean():
    """Runs a premake clean operation.
    """
    if sys.platform == 'darwin':
        return run_premake('macosx', 'clean')
    elif sys.platform == 'win32':
        return run_premake('windows', 'clean')
    else:
        return run_premake('linux', 'clean')


def run_platform_premake():
    """Runs all gyp configurations.
    """
    if sys.platform == 'darwin':
        return run_premake('macosx', 'xcode4')
    elif sys.platform == 'win32':
        vs_version = '2015'
        if 'VSVERSION' in os.environ:
            vs_version = os.environ['VSVERSION']

        return run_premake('windows', 'vs' + vs_version)
    else:
        ret = run_premake('linux', 'gmake')
        ret = ret != 0 and run_premake('linux', 'codelite') or ret
        return ret


def run_premake_export_commands():
    """Runs premake to generate an LLVM compile_commands.json file.
    """
    # TODO(benvanik): only do linux? whatever clang-tidy is ok with.
    if sys.platform == 'darwin':
        run_premake('macosx', 'export-compile-commands')
    elif sys.platform == 'win32':
        run_premake('windows', 'export-compile-commands')
    else:
        run_premake('linux', 'export-compile-commands')


def get_build_bin_path(args):
    """Returns the path of the bin/ path with build results based on the
    configuration specified in the parsed arguments.

    Args:
      args: Parsed arguments.

    Returns:
      A full path for the bin folder.
    """
    if sys.platform == 'darwin':
        platform = 'macosx'
    elif sys.platform == 'win32':
        platform = 'windows'
    else:
        platform = 'linux'
    return os.path.join(self_path, 'build', 'bin', platform, args['config'])


def discover_commands(subparsers):
    """Looks for all commands and returns a dictionary of them.
    In the future commands could be discovered on disk.

    Args:
      subparsers: Argument subparsers parent used to add command parsers.

    Returns:
      A dictionary containing name-to-Command mappings.
    """
    commands = {
        'setup': SetupCommand(subparsers),
        'pull': PullCommand(subparsers),
        'cmake': CmakeCommand(subparsers),
        'build': BuildCommand(subparsers),
        # 'genspirv': GenSpirvCommand(subparsers),
        # 'gentests': GenTestsCommand(subparsers),
        # 'test': TestCommand(subparsers),
        # 'gputest': GpuTestCommand(subparsers),
        # 'clean': CleanCommand(subparsers),
        # 'nuke': NukeCommand(subparsers),
        'lint': LintCommand(subparsers),
        'format': FormatCommand(subparsers),
        'style': StyleCommand(subparsers),
        # 'tidy': TidyCommand(subparsers),
    }
    if sys.platform == 'win32':
        commands['devenv'] = DevenvCommand(subparsers)
    return commands


class Command(object):
    """Base type for commands.
    """

    def __init__(self, subparsers, name, help_short=None, help_long=None,
                 *args, **kwargs):
        """Initializes a command.

        Args:
          subparsers: Argument subparsers parent used to add command parsers.
          name: The name of the command exposed to the management script.
          help_short: Help text printed alongside the command when queried.
          help_long: Extended help text when viewing command help.
        """
        self.name = name
        self.help_short = help_short
        self.help_long = help_long

        self.parser = subparsers.add_parser(name,
                                            help=help_short,
                                            description=help_long)
        self.parser.set_defaults(command_handler=self)

    def execute(self, args, pass_args, cwd):
        """Executes the command.

        Args:
          args: Arguments hash for the command.
          pass_args: Arguments list to pass to child commands.
          cwd: Current working directory.

        Returns:
          Return code of the command.
        """
        return 1


class SetupCommand(Command):
    """'setup' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(SetupCommand, self).__init__(
            subparsers,
            name='setup',
            help_short='Setup the build environment.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Setting up the build environment...')
        print('')

        # Setup submodules.
        print('- git submodule init / update...')
        git_submodule_update()
        print('- extracing openssl...')
        platform_openssl_extract()
        print('- precompiling luajit...')
        platform_luajit_compile()
        print('')

        print('- running cmake...')
        if run_platform_cmake() == 0:
            print('')
            print('Success!')

        return 0


class PullCommand(Command):
    """'pull' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(PullCommand, self).__init__(
            subparsers,
            name='pull',
            help_short='Pulls the repo and all dependencies and rebases changes.',
            *args, **kwargs)
        self.parser.add_argument('--merge', action='store_true',
                                 help='Merges on master instead of rebasing.')

    def execute(self, args, pass_args, cwd):
        print('Pulling...')
        print('')

        print('- switching to develop...')
        shell_call([
            'git',
            'checkout',
            'develop',
        ])
        print('')

        print('- pulling self...')
        if args['merge']:
            shell_call([
                'git',
                'pull',
            ])
        else:
            shell_call([
                'git',
                'pull',
                '--rebase',
            ])
        print('')

        print('- pulling dependencies...')
        git_submodule_update()
        print('')

        print('- running cmake...')
        if run_platform_cmake() == 0:
            print('')
            print('Success!')

        return 0


class CmakeCommand(Command):
    """'cmake' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(CmakeCommand, self).__init__(
            subparsers,
            name='cmake',
            help_short='Runs cmake to update all projects.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        # Update premake. If no binary found, it will be built from source.
        print('Running cmake...')
        print('')
        if run_platform_cmake() == 0:
            print('Success!')

        return 0


class BaseBuildCommand(Command):
    """Base command for things that require building."""

    def __init__(self, subparsers, *args, **kwargs):
        super(BaseBuildCommand, self).__init__(
            subparsers,
            *args, **kwargs)
        self.parser.add_argument(
            '--config', choices=['checked', 'debug', 'release'], default='debug',
            type=str.lower, help='Chooses the build configuration.')
        self.parser.add_argument(
            '--target', action='append', default=[],
            help='Builds only the given target(s).')
        self.parser.add_argument(
            '--force', action='store_true',
            help='Forces a full rebuild.')
        self.parser.add_argument(
            '--no_cmake', action='store_true',
            help='Skips running cmake before building.')
    
    def execute(self, args, pass_args, cwd):
        if not args['no_cmake']:
            print('- running cmake...')
            run_platform_cmake()
            print('')

        print('- building (%s):%s...' % (
            'all' if not len(args['target']) else ', '.join(args['target']),
            args['config']))
        if sys.platform == 'win32':
            targets = None
            if len(args['target']):
                targets = '/t:' + ';'.join(target + (':Rebuild' if args['force'] else '')
                                           for target in args['target'])
            else:
                targets = '/t:Rebuild' if args['force'] else None

            result = subprocess.call([
                'msbuild',
                os.path.join(self_path, 'build', 'ECU.sln'),
                '/nologo',
                '/m',
                '/v:m',
                '/p:Configuration=' + args['config'],
            ] + ([targets] if targets is not None else []) + pass_args, shell=False)
        elif sys.platform == 'darwin':
            # TODO(benvanik): other platforms.
            print('ERROR: don\'t know how to build on this platform.')
        else:
            # TODO(benvanik): allow gcc?
            if 'CXX' not in os.environ:
                os.environ['CXX'] = 'clang++-3.8'
            if 'CC' not in os.environ:
                os.environ['CC'] = 'clang-3.8'

            result = subprocess.call([
                'make',
                '-j',
                '-Cbuild/',
                'config=%s_linux' % (args['config']),
            ] + pass_args + args['target'], shell=False)
        print('')
        if result != 0:
            print('ERROR: build failed with one or more errors.')
            return result
        return 0


class BuildCommand(BaseBuildCommand):
    """'build' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(BuildCommand, self).__init__(
            subparsers,
            name='build',
            help_short='Builds the project.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Building %s...' % (args['config']))
        print('')

        result = super(BuildCommand, self).execute(args, pass_args, cwd)
        if not result:
            print('Success!')
        return result


class GenSpirvCommand(Command):
    """'genspirv' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(GenSpirvCommand, self).__init__(
            subparsers,
            name='genspirv',
            help_short='Generates SPIR-V binaries and header files.',
            help_long='''
        Generates the .spv/.h binaries under src/xenia/*/vulkan/shaders/bin/).
        Run after modifying any .vert/.geom/.frag files.
        ''',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Generating SPIR-V binaries...')
        print('')

        vulkan_sdk_path = os.environ['VULKAN_SDK']
        vulkan_bin_path = os.path.join(vulkan_sdk_path, 'bin')
        glslang = os.path.join(vulkan_bin_path, 'glslangValidator')
        spirv_dis = os.path.join(vulkan_bin_path, 'spirv-dis')
        spirv_remap = os.path.join(vulkan_bin_path, 'spirv-remap')

        # Ensure we have the tools.
        if not os.path.exists(vulkan_sdk_path):
            print('ERROR: could not find the Vulkan SDK')
            return 1
        elif not has_bin(glslang):
            print('ERROR: could not find glslangValidator')
            return 1
        elif not has_bin(spirv_dis):
            print('ERROR: could not find spirv-dis')
            return 1
        elif not has_bin(spirv_remap):
            print('ERROR: could not find spirv-remap')
            return 1

        src_files = [os.path.join(root, name)
                     for root, dirs, files in os.walk('src')
                     for name in files
                     if (name.endswith('.vert') or name.endswith('.geom') or
                         name.endswith('.frag'))]

        any_errors = False
        for src_file in src_files:
            print('- %s' % (src_file))
            src_name = os.path.splitext(os.path.basename(src_file))[0]
            identifier = os.path.basename(src_file).replace('.', '_')

            bin_path = os.path.join(os.path.dirname(src_file), 'bin')
            spv_file = os.path.join(bin_path, identifier) + '.spv'
            txt_file = os.path.join(bin_path, identifier) + '.txt'
            h_file = os.path.join(bin_path, identifier) + '.h'

            # GLSL source -> .spv binary
            shell_call([
                glslang,
                '-V', src_file,
                '-o', spv_file,
            ])

            # Disassemble binary into human-readable text.
            shell_call([
                spirv_dis,
                '-o', txt_file,
                spv_file,
            ])

            # TODO(benvanik): remap?

            # bin2c so we get a header file we can compile in.
            with open(h_file, 'wb') as out_file:
                out_file.write('// generated from `xb genspirv`\n')
                out_file.write('// source: %s\n' % os.path.basename(src_file))
                out_file.write('const uint8_t %s[] = {' % (identifier))
                with open(spv_file, 'rb') as in_file:
                    index = 0
                    c = in_file.read(1)
                    while c != '':
                        if index % 12 == 0:
                            out_file.write('\n    ')
                        else:
                            out_file.write(' ')
                        index += 1
                        out_file.write('0x%02X,' % ord(c))
                        c = in_file.read(1)
                out_file.write('\n};\n')

        if any_errors:
            print('ERROR: failed to build one or more SPIR-V files.')
            return 1

        return 0


class TestCommand(BaseBuildCommand):
    """'test' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(TestCommand, self).__init__(
            subparsers,
            name='test',
            help_short='Runs automated tests that have been built with `xb build`.',
            help_long='''
        To pass arguments to the test executables separate them with `--`.
        For example, you can run only the instr_foo.s tests with:
          $ xb test -- instr_foo
        ''',
            *args, **kwargs)
        self.parser.add_argument(
            '--no_build', action='store_true',
            help='Don\'t build before running tests.')
        self.parser.add_argument(
            '--continue', action='store_true',
            help='Don\'t stop when a test errors, but continue running all.')

    def execute(self, args, pass_args, cwd):
        print('Testing...')
        print('')

        # The test executables that will be built and run.
        test_targets = args['target'] or [
            'xenia-cpu-ppc-tests',
        ]
        args['target'] = test_targets

        # Build all targets (if desired).
        if not args['no_build']:
            result = super(TestCommand, self).execute(args, [], cwd)
            if result:
                print('Failed to build, aborting test run.')
                return result

        # Ensure all targets exist before we run.
        test_executables = [
            get_bin(os.path.join(get_build_bin_path(args), test_target))
            for test_target in test_targets]
        for test_executable in test_executables:
            if not has_bin(test_executable):
                print('ERROR: Unable to find %s - build it.' %
                      (test_executable))
                return 1

        # Run tests.
        any_failed = False
        for test_executable in test_executables:
            print('- %s' % (test_executable))
            result = shell_call([
                test_executable,
            ] + pass_args,
                throw_on_error=False)
            if result:
                any_failed = True
                if args['continue']:
                    print('ERROR: test failed but continuing due to --continue.')
                else:
                    print('ERROR: test failed, aborting, use --continue to keep going.')
                    return result

        if any_failed:
            print('ERROR: one or more tests failed.')
            result = 1
        return result


class GenTestsCommand(Command):
    """'gentests' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(GenTestsCommand, self).__init__(
            subparsers,
            name='gentests',
            help_short='Generates test binaries.',
            help_long='''
        Generates test binaries (under src/xenia/cpu/ppc/testing/bin/).
        Run after modifying test .s files.
        ''',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Generating test binaries...')
        print('')

        binutils_path = os.path.join('third_party', 'binutils-ppc-cygwin')
        ppc_as = os.path.join(binutils_path, 'powerpc-none-elf-as')
        ppc_ld = os.path.join(binutils_path, 'powerpc-none-elf-ld')
        ppc_objdump = os.path.join(binutils_path, 'powerpc-none-elf-objdump')
        ppc_nm = os.path.join(binutils_path, 'powerpc-none-elf-nm')

        test_src = os.path.join('src', 'xenia', 'cpu', 'ppc', 'testing')
        test_bin = os.path.join(test_src, 'bin')

        # Ensure the test output path exists.
        if not os.path.exists(test_bin):
            os.mkdir(test_bin)

        src_files = [os.path.join(root, name)
                     for root, dirs, files in os.walk('src')
                     for name in files
                     if (name.startswith('instr_') or name.startswith('seq_'))
                     and name.endswith(('.s'))]

        def make_unix_path(p):
            """Forces a unix path separator style, as required by binutils.
            """
            return string.replace(p, os.sep, '/')

        any_errors = False
        for src_file in src_files:
            print('- %s' % (src_file))
            src_name = os.path.splitext(os.path.basename(src_file))[0]
            obj_file = os.path.join(test_bin, src_name) + '.o'
            shell_call([
                ppc_as,
                '-a32',
                '-be',
                '-mregnames',
                '-mpower7',
                '-maltivec',
                '-mvsx',
                '-mvmx128',
                '-R',
                '-o%s' % (make_unix_path(obj_file)),
                make_unix_path(src_file),
            ])
            dis_file = os.path.join(test_bin, src_name) + '.dis'
            shell_call([
                ppc_objdump,
                '--adjust-vma=0x100000',
                '-Mpower7',
                '-Mvmx128',
                '-D',
                '-EB',
                make_unix_path(obj_file),
            ], stdout_path=dis_file)
            # Eat the first 4 lines to kill the file path that'll differ across
            # machines.
            with open(dis_file) as f:
                dis_file_lines = f.readlines()
            with open(dis_file, 'w') as f:
                f.writelines(dis_file_lines[4:])
            shell_call([
                ppc_ld,
                '-A powerpc:common32',
                '-melf32ppc',
                '-EB',
                '-nostdlib',
                '--oformat=binary',
                '-Ttext=0x80000000',
                '-e0x80000000',
                '-o%s' % (make_unix_path(os.path.join(test_bin, src_name) + '.bin')),
                make_unix_path(obj_file),
            ])
            shell_call([
                ppc_nm,
                '--numeric-sort',
                make_unix_path(obj_file),
            ], stdout_path=os.path.join(test_bin, src_name) + '.map')

        if any_errors:
            print('ERROR: failed to build one or more tests.')
            return 1

        return 0


class GpuTestCommand(BaseBuildCommand):
    """'gputest' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(GpuTestCommand, self).__init__(
            subparsers,
            name='gputest',
            help_short='Runs automated GPU diff tests against reference imagery.',
            help_long='''
        To pass arguments to the test executables separate them with `--`.
        ''',
            *args, **kwargs)
        self.parser.add_argument(
            '--no_build', action='store_true',
            help='Don\'t build before running tests.')
        self.parser.add_argument(
            '--update_reference_files', action='store_true',
            help='Update all reference imagery.')
        self.parser.add_argument(
            '--generate_missing_reference_files', action='store_true',
            help='Create reference files for new traces.')

    def execute(self, args, pass_args, cwd):
        print('Testinging...')
        print('')

        # The test executables that will be built and run.
        test_targets = args['target'] or [
            'xenia-gpu-gl4-trace-dump',
        ]
        args['target'] = test_targets

        # Build all targets (if desired).
        if not args['no_build']:
            result = super(GpuTestCommand, self).execute(args, [], cwd)
            if result:
                print('Failed to build, aborting test run.')
                return result

        # Ensure all targets exist before we run.
        test_executables = [
            get_bin(os.path.join(get_build_bin_path(args), test_target))
            for test_target in test_targets]
        for test_executable in test_executables:
            if not has_bin(test_executable):
                print('ERROR: Unable to find %s - build it.' %
                      (test_executable))
                return 1

        output_path = os.path.join(self_path, 'build', 'gputest')
        if os.path.isdir(output_path):
            shutil.rmtree(output_path)
        os.makedirs(output_path)
        print('Running tests and outputting to %s...' % (output_path))

        reference_trace_root = os.path.join(self_path, 'testdata',
                                            'reference-gpu-traces')

        # Run tests.
        any_failed = False
        result = shell_call([
            'python',
            os.path.join(self_path, 'tools', 'gpu-trace-diff'),
            '--executable=' + test_executables[0],
            '--trace_path=' + os.path.join(reference_trace_root, 'traces'),
            '--output_path=' + output_path,
            '--reference_path=' +
            os.path.join(reference_trace_root, 'references'),
        ] + (['--generate_missing_reference_files']
             if args['generate_missing_reference_files'] else []) +
            (['--update_reference_files']
             if args['update_reference_files'] else []) +
            pass_args,
            throw_on_error=False)
        if result:
            any_failed = True

        if any_failed:
            print('ERROR: one or more tests failed.')
            result = 1
        print('Check %s/results.html for more details.' % (output_path))
        return result


class CleanCommand(Command):
    """'clean' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(CleanCommand, self).__init__(
            subparsers,
            name='clean',
            help_short='Removes intermediate files and build outputs.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Cleaning build artifacts...')
        print('')

        print('- premake clean...')
        run_premake_clean()
        run_cmake_clean()
        print('')

        print('Success!')
        return 0


class NukeCommand(Command):
    """'nuke' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(NukeCommand, self).__init__(
            subparsers,
            name='nuke',
            help_short='Removes all build/ output.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Cleaning build artifacts...')
        print('')

        print('- removing build/...')
        if os.path.isdir('build/'):
            shutil.rmtree('build/')
        print('')

        print('- git reset to master...')
        shell_call([
            'git',
            'reset',
            '--hard',
            'master',
        ])
        print('')

        print('- running premake...')
        run_platform_premake()
        print('')

        print('Success!')
        return 0


def find_ecu_source_files():
    """Gets all xenia source files in the project.

    Returns:
      A list of file paths.
    """
    return [os.path.join(root, name)
            for root, dirs, files in os.walk('src')
            for name in files
            if name.endswith(('.cc', '.c', '.h', '.inl'))]


def find_all_source_files():
    """Gets all interesting source files in the project.

    Returns:
      A list of file paths.
    """
    return find_ecu_source_files()


class LintCommand(Command):
    """'lint' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(LintCommand, self).__init__(
            subparsers,
            name='lint',
            help_short='Checks for lint errors with clang-format.',
            *args, **kwargs)
        self.parser.add_argument(
            '--all', action='store_true',
            help='Lint all files, not just those changed.')
        self.parser.add_argument(
            '--origin', action='store_true',
            help='Lints all files changed relative to origin/develop.')

    def execute(self, args, pass_args, cwd):
        clang_format_binary = get_clang_format_binary()

        difftemp = '.difftemp.txt'

        if args['all']:
            all_files = find_all_source_files()
            print('- linting %d files' % (len(all_files)))
            any_errors = False
            for file_path in all_files:
                if os.path.exists(difftemp):
                    os.remove(difftemp)
                ret = shell_call([
                    clang_format_binary,
                    '-output-replacements-xml',
                    '-style=file',
                    file_path,
                ], throw_on_error=False, stdout_path=difftemp)
                with open(difftemp) as f:
                    had_errors = '<replacement ' in f.read()
                if os.path.exists(difftemp):
                    os.remove(difftemp)
                if had_errors:
                    any_errors = True
                    print('')
                    print(file_path)
                    shell_call([
                        clang_format_binary,
                        '-style=file',
                        file_path,
                    ], throw_on_error=False, stdout_path=difftemp)
                    shell_call([
                        'python',
                        'tools/diff.py',
                        file_path,
                        difftemp,
                        difftemp,
                    ])
                    shell_call([
                        'type' if sys.platform == 'win32' else 'cat',
                        difftemp,
                    ])
                    if os.path.exists(difftemp):
                        os.remove(difftemp)
                    print('')
            print('')
            if any_errors:
                print('ERROR: 1+ diffs. Stage changes and run \'ecu format\' to fix.')
                return 1
            else:
                print('Linting completed successfully.')
                return 0
        else:
            print('- git-clang-format --diff')
            if os.path.exists(difftemp):
                os.remove(difftemp)
            ret = shell_call([
                'python',
                'extern/clang-format/git-clang-format',
                '--binary=%s' % (clang_format_binary),
                '--commit=%s' % (
                    'origin/develop' if args['origin'] else 'HEAD'),
                '--diff',
            ], throw_on_error=False, stdout_path=difftemp)
            with open(difftemp) as f:
                contents = f.read()
                not_modified = 'no modified files' in contents
                not_modified = not_modified or 'did not modify' in contents
                f.close()
            if os.path.exists(difftemp):
                os.remove(difftemp)
            if not not_modified:
                any_errors = True
                print('')
                shell_call([
                    'python',
                    'extern/clang-format/git-clang-format',
                    '--binary=%s' % (clang_format_binary),
                    '--commit=%s' % (
                        'origin/develop' if args['origin'] else 'HEAD'),
                    '--diff',
                ])
                print('ERROR: 1+ diffs. Stage changes and run \'ecu format\' to fix.')
                return 1
            else:
                print('Linting completed successfully.')
                return 0


class FormatCommand(Command):
    """'format' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(FormatCommand, self).__init__(
            subparsers,
            name='format',
            help_short='Reformats staged code with clang-format.',
            *args, **kwargs)
        self.parser.add_argument(
            '--all', action='store_true',
            help='Format all files, not just those changed.')
        self.parser.add_argument(
            '--origin', action='store_true',
            help='Formats all files changed relative to origin/develop.')

    def execute(self, args, pass_args, cwd):
        clang_format_binary = get_clang_format_binary()

        if args['all']:
            all_files = find_all_source_files()
            print('- clang-format [%d files]' % (len(all_files)))
            any_errors = False
            for file_path in all_files:
                ret = shell_call([
                    clang_format_binary,
                    '-i',
                    '-style=file',
                    file_path,
                ], throw_on_error=False)
                if ret:
                    any_errors = True
            print('')
            if any_errors:
                print('ERROR: 1+ clang-format calls failed.')
                print('Ensure all files are staged.')
                return 1
            else:
                print('Formatting completed successfully.')
                return 0
        else:
            print('- git-clang-format')
            shell_call([
                'python',
                'extern/clang-format/git-clang-format',
                '--binary=%s' % (clang_format_binary),
                '--commit=%s' % (
                    'origin/develop' if args['origin'] else 'HEAD'),
            ])
            print('')

        return 0


# TODO(benvanik): merge into linter, or as lint --anal?
class StyleCommand(Command):
    """'style' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(StyleCommand, self).__init__(
            subparsers,
            name='style',
            help_short='Runs the style checker on all code.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        all_files = [file_path for file_path in find_all_source_files()
                     if not file_path.endswith('_test.cc')]
        print('- cpplint [%d files]' % (len(all_files)))
        ret = shell_call([
            'python',
            'extern/google-styleguide/cpplint/cpplint.py',
            '--output=vs7',
            '--linelength=90',
            '--filter=-build/c++11,+build/include_alpha',
            '--root=src',
        ] + all_files, throw_on_error=False)
        print('')
        if ret:
            print('ERROR: 1+ cpplint calls failed.')
            return 1
        else:
            print('Style linting completed successfully.')
            return 0

        return 0


# TODO(benvanik): merge into linter, or as lint --anal?
class TidyCommand(Command):
    """'tidy' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(TidyCommand, self).__init__(
            subparsers,
            name='tidy',
            help_short='Runs the clang-tidy checker on all code.',
            *args, **kwargs)
        self.parser.add_argument(
            '--fix', action='store_true',
            help='Applies suggested fixes, where possible.')

    def execute(self, args, pass_args, cwd):
        # Run premake to generate our compile_commands.json file for clang to
        # use.
        run_premake_export_commands()

        platform_name = ''
        if sys.platform == 'darwin':
            platform_name = 'darwin'
        elif sys.platform == 'win32':
            platform_name = 'windows'
        else:
            platform_name = 'linux'
        tool_root = 'build/llvm_tools/debug_%s' % (platform_name)

        all_files = [file_path for file_path in find_all_source_files()
                     if not file_path.endswith('_test.cc')]
        # Tidy only likes .cc files.
        all_files = [file_path for file_path in all_files
                     if file_path.endswith('.cc')]

        any_errors = False
        for file in all_files:
            print('- clang-tidy %s' % (file))
            ret = shell_call([
                'clang-tidy',
                '-p', tool_root,
                '-checks=' + ','.join([
                    'clang-analyzer-*',
                    'google-*',
                    'misc-*',
                    'modernize-*'
                    # TODO(benvanik): pick the ones we want - some are silly.
                    # 'readability-*',
                ]),
            ] + (['-fix'] if args['fix'] else []) + [
                file,
            ], throw_on_error=False)
            if ret:
                any_errors = True

        print('')
        if any_errors:
            print('ERROR: 1+ clang-tidy calls failed.')
            return 1
        else:
            print('Tidy completed successfully.')
            return 0

        return 0


class DevenvCommand(Command):
    """'devenv' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(DevenvCommand, self).__init__(
            subparsers,
            name='devenv',
            help_short='Launches Visual Studio with the sln.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Launching Visual Studio...')
        print('')

        print('- running cmake...')
        run_platform_cmake()
        print('')

        print('- launching devenv...')
        shell_call([
            'devenv',
            os.path.join(self_path, 'build', 'ECU.sln'),
        ])
        print('')

        return 0


if __name__ == '__main__':
    main()
