# Build helpers shared by the SConstruct file: the base compiler settings and
# the small files the build writes while it sets itself up.

import os
import shutil

# Warning and language settings the old build passed to Clang on Windows.
CLANG_FLAGS = [
    '/W3',
    '/clang:-ferror-limit=0',
    '/clang:-Wno-writable-strings',
    '/clang:-Wno-deprecated-declarations',
    '/clang:-Wno-microsoft-enum-value',
    '/clang:-fno-operator-names',
    '/clang:-fno-strict-aliasing',
    '/clang:-fwrapv',
]

# Definitions the old build set for the whole project.
GLOBAL_DEFINES = [
    'WIN32',
    'C_ONLY',
    '_CRT_SECURE_NO_WARNINGS',
    '_CRT_NONSTDC_NO_WARNINGS',
    'NOMINMAX',
]

# The engine sources include each other with the folder names of the old
# tree, where game, cgame, ui, botlib, win32 and the bundled libraries sat
# directly inside code/. The folders have moved (game/game, game/cgame,
# sys/win32, third-party/...), but the includes were never rewritten. These
# include folders make the old include paths find the real files again, so
# no engine source has to be touched.
LEGACY_INCLUDE_PATHS = [
    '#code/game',              # "game/x.h", "cgame/x.h", "ui/x.h", "botlib/x.h"
    '#code/game/cgame',        # "fx/x.h" from the cgame tests
    '#code/game/game',         # "../game/x.h", "../cgame/x.h", "../ui/x.h", ...
    '#code/game/game/tests',   # "../../game/x.h" from the aasbuild compiler
    '#code/client',            # "../qcommon/x.h", "../renderer/x.h", ... from moved files
    '#code/sys',               # "win32/x.h"
    '#code/third-party/SDL3/include',  # <SDL3/SDL.h> for the sys layer
    '#code/third-party',       # "openal/x.h", "fluidsynth/x.h", ...
]

# Warning and language settings for the Linux build, matching what the
# Windows flags do: full warnings, nothing pedantic beyond that.
GCC_FLAGS = [
    '-Wall',
    '-Wno-unused-function',
    '-fno-strict-aliasing',
    '-fwrapv',
]


def pkg_config_libs(name):
    """The linker flags for a system library, straight from pkg-config."""
    import subprocess
    return subprocess.check_output(
        ['pkg-config', '--libs', name], text=True).split()


def make_base_env(config, arch='x86'):
    """The starting point for every other environment in the build.

    The project builds with Clang through the MSVC command line (clang-cl).
    When clang-cl is missing, plain MSVC is used as a fallback.
    """
    from SCons.Script import Environment

    if os.name != 'nt':
        return make_base_env_linux(config)

    env = Environment(tools=['default'],
                      TARGET_ARCH='amd64' if arch == 'x64' else 'x86')

    if shutil.which('clang-cl'):
        env['CC'] = 'clang-cl'
        env['CXX'] = 'clang-cl'
        # clang-cl follows the host by default, so point it at the word
        # size the build asks for.
        triple = ('x86_64-pc-windows-msvc' if arch == 'x64'
                  else 'i686-pc-windows-msvc')
        env['CCFLAGS'] = ['--target=' + triple] + CLANG_FLAGS
    else:
        print('clang-cl was not found, using the MSVC compiler instead.')
        print('The project normally builds with Clang, so expect problems.')
        # cl prints its banner for every file it compiles; /nologo keeps
        # the fallback build output readable.
        env['CCFLAGS'] = ['/nologo']

    # /EHsc turns C++ exceptions on: the editor format readers use throw.
    env['CXXFLAGS'] = ['/std:c++17', '/EHsc']
    env['SHLIBPREFIX'] = ''
    env.Append(CPPDEFINES=GLOBAL_DEFINES)
    if arch == 'x86':
        # The 32-bit build keeps the historical 32-bit time_t. The 64-bit
        # build has no such switch, its time_t is 64-bit.
        env.Append(CPPDEFINES=['_USE_32BIT_TIME_T'])
    # Include folders that make the old include paths work, see above.
    env.Append(CPPPATH=LEGACY_INCLUDE_PATHS)

    if config == 'debug':
        # /Z7 keeps the debug info inside the object files, so parallel
        # compiles cannot fight over one shared pdb file.
        env.Append(CCFLAGS=['/MTd', '/Z7', '/Od'], LINKFLAGS=['/DEBUG'])
    else:
        env.Append(CCFLAGS=['/MT', '/O2'], CPPDEFINES=['NDEBUG'])

    return env


def make_base_env_linux(config):
    """The base environment for the Linux build: gcc with the SDL3 that
    pkg-config finds. The sources are the same as on Windows, only the
    platform files differ."""
    import subprocess
    from SCons.Script import Environment

    env = Environment(tools=['default'])

    env['CCFLAGS'] = GCC_FLAGS + ['-fPIC']
    env['CXXFLAGS'] = ['-std=c++17', '-fexceptions']
    env['SHLIBPREFIX'] = ''
    # The same definitions the Windows build sets, minus the ones that
    # only mean something to the Microsoft headers.
    env.Append(CPPDEFINES=['C_ONLY', 'NOMINMAX'])

    if config == 'debug':
        env.Append(CCFLAGS=['-g', '-O0'])
    else:
        env.Append(CCFLAGS=['-O2'], CPPDEFINES=['NDEBUG'])

    # SDL3 comes from the pinned source CI builds (or a system package);
    # pkg-config knows where its headers are.
    cflags = subprocess.check_output(
        ['pkg-config', '--cflags', 'sdl3'], text=True).split()
    env.Append(CCFLAGS=cflags)

    # Include folders that make the old include paths work, see above.
    env.Append(CPPPATH=LEGACY_INCLUDE_PATHS)

    return env


def write_generated_files(build_root, arch='x86', code_dir='code'):
    """Write the small generated files the build needs.

    Returns the folder they end up in, as a real path.
    """
    generated = os.path.join(build_root, 'generated')
    os.makedirs(generated, exist_ok=True)

    # The editor DLL exports the compiler callback through this def file.
    # 32-bit Windows decorates the stdcall callback name; 64-bit Windows has
    # no stdcall decoration, so the plain symbol name is exported there.
    if arch == 'x64':
        callback_symbol = 'DoomTools_RunW'
    else:
        callback_symbol = '_DoomTools_RunW@16'
    with open(os.path.join(code_dir, 'tools', 'compilers', 'exports.def.in')) as f:
        template = f.read()
    text = template.replace('@DOOM_COMPILER_CALLBACK_SYMBOL@', callback_symbol)
    with open(os.path.join(generated, 'compiler-exports.def'), 'w') as f:
        f.write(text)

    return generated


def fetch_openal_soft(download_dir):
    """Download the OpenAL Soft sources for the non-Windows builds.

    The vendored copy in code/third-party/openal is the prebuilt Windows
    package (headers, import libraries and DLLs). Other systems fetch the
    pinned 1.23.1 release here, so the sources are ready before the port
    actually builds on them. The download is skipped when it already ran
    once.
    """
    import tarfile
    import urllib.request

    version = '1.23.1'
    extracted = os.path.join(download_dir, 'openal-soft-' + version)
    if os.path.isdir(extracted):
        return extracted

    os.makedirs(download_dir, exist_ok=True)
    url = ('https://github.com/kcat/openal-soft/archive/refs/tags/'
           + version + '.tar.gz')
    archive = os.path.join(download_dir, 'openal-soft-' + version + '.tar.gz')
    print('Fetching OpenAL Soft %s from %s' % (version, url))
    urllib.request.urlretrieve(url, archive)

    with tarfile.open(archive) as tar:
        # Python 3.12 warns about unsafe member paths; older versions do
        # not know the filter argument yet.
        try:
            tar.extractall(download_dir, filter='data')
        except TypeError:
            tar.extractall(download_dir)

    with open(os.path.join(download_dir, 'README-openal-soft.txt'), 'w') as f:
        f.write('OpenAL Soft %s sources, fetched automatically by SCons.\n'
                'They are used by the non-Windows builds (task 4).\n'
                % version)
    return extracted


def fetch_fluidsynth(download_dir):
    """Download the FluidSynth sources for the non-Windows builds.

    The vendored copy in code/third-party/fluidsynth ships the headers on
    every system and the Windows binaries are loaded at runtime, so only the
    non-Windows builds need the sources. They fetch the pinned 2.6.1 release
    here. The download is skipped when it already ran once.
    """
    import tarfile
    import urllib.request

    version = '2.6.1'
    extracted = os.path.join(download_dir, 'fluidsynth-' + version)
    if os.path.isdir(extracted):
        return extracted

    os.makedirs(download_dir, exist_ok=True)
    url = ('https://github.com/FluidSynth/fluidsynth/archive/refs/tags/'
           'v' + version + '.tar.gz')
    archive = os.path.join(download_dir, 'fluidsynth-' + version + '.tar.gz')
    print('Fetching FluidSynth %s from %s' % (version, url))
    urllib.request.urlretrieve(url, archive)

    with tarfile.open(archive) as tar:
        # Python 3.12 warns about unsafe member paths; older versions do
        # not know the filter argument yet.
        try:
            tar.extractall(download_dir, filter='data')
        except TypeError:
            tar.extractall(download_dir)

    with open(os.path.join(download_dir, 'README-fluidsynth.txt'), 'w') as f:
        f.write('FluidSynth %s sources, fetched automatically by SCons.\n'
                'They are used by the non-Windows builds (task 4).\n'
                % version)
    return extracted


def run_tests(target, source, env):
    """Run every test program in source, called by "scons test"."""
    import subprocess

    # The TESTS list in doom_sources.py carries the command line each
    # test needs, the runner looks it up by executable name.
    test_args = env.get('TEST_ARGS') or {}
    failed = []
    for test in source:
        name = os.path.basename(test.path)
        if name.endswith('.exe'):
            name = name[:-len('.exe')]
        print('Running %s' % name)
        code = subprocess.call([test.abspath] + list(test_args.get(name, [])))
        if code != 0:
            failed.append(name)

    if failed:
        return 'Failed tests: ' + ', '.join(failed)

    with open(target[0].abspath, 'w') as f:
        f.write('All tests passed.\n')
    return None

