from os import getcwd
import os.path as path
import os
import shutil
import sys
from subprocess import Popen
from pathlib import Path

from justenv import MESON, MESON_CROSS_DIR, ENVIRON, DELIVER_DIR
from vsutil import VCArch
import vsutil
from msysutil import MSYS_ENV
from dyninfo import VERSION

DESCRIPTION = 'Uma biblioteca com funções semelhantes às do C '\
'para fins educacionais na Eletrônica, IF Sul Pelotas.'

DIST_DIR = path.join(getcwd(), 'dist')
INSTALL_COMBO = path.join(DIST_DIR, 'install-combo')

def run_proc(
        exe: str, args: list[str], env: dict[str, str] | None = None,
        base_env: dict[str, str] | None = None
    ) -> int:

    if base_env == None:
        penv = ENVIRON
    else:
        penv = base_env

    if env != None:
        for k, v in env.items():
            print(f'{k}={v}')

        penv.update(env)


    print(f'{exe} {" ".join(args)}')
    return Popen([exe, *args], env=penv).wait()

def reset_dist():
    shutil.rmtree(DIST_DIR, ignore_errors=True)
    os.mkdir(DIST_DIR)
    with open(path.join(DIST_DIR, '.gitignore'), 'wb') as GITIGNORE:
        GITIGNORE.write(bytes('*\n', encoding='utf8'))

def build_target(target: str, prefix: str, destdir = INSTALL_COMBO, build_env: dict[str, str] | None = None) -> int:
    CROSS_FILE = path.relpath(path.join(MESON_CROSS_DIR, target + '.ini'), getcwd())
    BUILD_DIR = path.relpath(path.join(DIST_DIR, target), getcwd())

    SETUP_ARGS = [
        'setup',
        '--cross-file', CROSS_FILE,
        '--prefix', prefix,
        '--buildtype', 'release',
        BUILD_DIR,
    ]

    INSTALL_ARGS = [
        'install',
        '-C', BUILD_DIR,
    ]
    INSTALL_ENV = {
        'DESTDIR': destdir,
    }

    exit_code = run_proc(MESON, SETUP_ARGS, base_env=build_env)
    if exit_code != 0:
        return exit_code
    exit_code = run_proc(MESON, INSTALL_ARGS, base_env=build_env, env=INSTALL_ENV)
    if exit_code != 0:
        return exit_code

    shutil.rmtree(BUILD_DIR)

    return 0


def linux_gnu() -> int:
    reset_dist()

    PREFIX = '/usr/local'
    COMBO_PREFIX = path.normpath(INSTALL_COMBO + PREFIX)

    TARGETS = [
        'x86_64-linux-gnu',
        'x86-linux-gnu',
        'aarch64-linux-gnu',
        'arm-linux-gnueabi',
    ]

    exit_status = 0
    for TARGET in TARGETS:
        exit_status += build_target(TARGET, PREFIX)

    if exit_status != 0:
        return exit_status

    BINDIR = path.join(COMBO_PREFIX, 'bin')
    Path(BINDIR).mkdir(parents=True, exist_ok=True)

    TROVSCODE_SRC = path.join(DELIVER_DIR, 'tro-vscode')
    TROVSCODE_DEST = path.join(BINDIR, 'tro-vscode')
    shutil.copyfile(TROVSCODE_SRC, TROVSCODE_DEST)
    os.chmod(TROVSCODE_DEST, mode=0o777)

    FPM = shutil.which('fpm')
    if FPM == None:
        raise Exception('fpm não encontrado')

    ARCHIVE = path.join(DIST_DIR, f'libtro_{VERSION}_linux-gnu')
    ARCHIVE_TARXZ = shutil.make_archive(ARCHIVE, 'xztar', INSTALL_COMBO)
    ARCHIVE_DEB = ARCHIVE + '.deb'
    ARCHIVE_RPM = ARCHIVE + '.rpm'

    exit_status += run_proc(FPM, [
        '--input-type', 'tar',
        '--output-type', 'deb',
        '--name', 'libtro',
        '--version', VERSION,
        '--description', DESCRIPTION,
        '--architecture', 'noarch',
        '--package', ARCHIVE_DEB,
        ARCHIVE_TARXZ,
    ])

    exit_status += run_proc(FPM, [
        '--input-type', 'tar',
        '--output-type', 'rpm',
        '--name', 'libtro',
        '--version', VERSION.replace('-', '_'),
        '--description', DESCRIPTION,
        '--architecture', 'noarch',
        '--package', ARCHIVE_RPM,
        ARCHIVE_TARXZ,
    ])

    if exit_status != 0:
        return exit_status

    shutil.rmtree(INSTALL_COMBO, ignore_errors=True)

    return 0


def pc_windows() -> int:
    reset_dist()

    PREFIX = 'C:\\LibTRO'
    COMBO_PREFIX = path.normpath(INSTALL_COMBO + PREFIX.replace('C:', ''))

    exit_status = 0

    # Manter sincronizado com o LibTRO.props e com ajuda.cmd.
    VC_TARGETS = [
        ('x86_64-pc-windows-msvc', VCArch.X64),
        ('x86-pc-windows-msvc', VCArch.X86),
        ('aarch64-pc-windows-msvc', VCArch.ARM64),
    ]

    for TARGET, VC_ARCH in VC_TARGETS:
        VC_ENV = vsutil.vcvars(VC_ARCH)
        exit_status += build_target(TARGET, PREFIX, build_env=VC_ENV)

    # Manter sincronizado com o ajuda.cmd.
    GNU_TARGETS = [
        'x86_64-pc-windows-gnu',
        'i686-pc-windows-gnu',
        'aarch64-pc-windows-gnu',
    ]

    for TARGET in GNU_TARGETS:
        exit_status += build_target(TARGET, PREFIX, build_env=MSYS_ENV)

    if exit_status != 0:
        return exit_status

    PROPS_SRC = path.join(DELIVER_DIR, 'LibTRO.props')
    PROPS_DEST = path.join(COMBO_PREFIX, 'LibTRO.props')
    shutil.copyfile(PROPS_SRC, PROPS_DEST)

    AJUDACMD_SRC = path.join(DELIVER_DIR, 'ajuda.cmd')
    AJUDACMD_DEST = path.join(COMBO_PREFIX, 'ajuda.cmd')
    shutil.copyfile(AJUDACMD_SRC, AJUDACMD_DEST)

    BINDIR = path.join(COMBO_PREFIX, 'bin')
    Path(BINDIR).mkdir(parents=True, exist_ok=True)

    TROVSCODE_SRC = path.join(DELIVER_DIR, 'tro-vscode.cmd')
    TROVSCODE_DEST = path.join(BINDIR, 'tro-vscode.cmd')
    shutil.copyfile(TROVSCODE_SRC, TROVSCODE_DEST)

    ARCHIVE = path.join(DIST_DIR, f'libtro_{VERSION}_pc-windows')
    shutil.make_archive(ARCHIVE, 'zip', INSTALL_COMBO)
    shutil.rmtree(INSTALL_COMBO, ignore_errors=True)

    return 0


def apple_darwin() -> int:
    reset_dist()

    PREFIX = '/usr/local'

    ARCHS = [
        'x86_64',
        'aarch64',
    ]
    DESTDIRS = list(map(lambda arch: path.join(INSTALL_COMBO, arch), ARCHS))
    TARGETS = list(map(lambda arch: f'{arch}-apple-darwin', ARCHS))

    exit_status = 0
    for TARGET, DESTDIR in zip(TARGETS, DESTDIRS):
        exit_status += build_target(TARGET, PREFIX, destdir=DESTDIR)

    if exit_status != 0:
        return exit_status

    GENERIC_DEST_PREFIX = path.normpath(DESTDIRS[0] + PREFIX)
    COMBO_PREFIX = path.normpath(INSTALL_COMBO + PREFIX)

    SRC_INCLUDE = path.join(GENERIC_DEST_PREFIX, 'include')
    DEST_INCLUDE = path.join(COMBO_PREFIX, 'include')
    shutil.copytree(SRC_INCLUDE, DEST_INCLUDE)

    LIBDIR = path.join(COMBO_PREFIX, 'lib')
    PKGCFGDIR = path.join(LIBDIR, 'pkgconfig')
    Path(PKGCFGDIR).mkdir(parents=True, exist_ok=True)

    SRC_TROPC = path.join(DESTDIRS[0] + PREFIX, 'lib', 'pkgconfig', 'tro.pc')
    DEST_TROPC = path.join(PKGCFGDIR, 'tro.pc')
    shutil.copyfile(SRC_TROPC, DEST_TROPC)

    LIPO = shutil.which('lipo')
    if LIPO == None:
        raise Exception('lipo não encontrado')

    LIPO_ARGS =  ['-create', '-output', path.join(LIBDIR, 'libtro.a')]
    for ARCH, DESTDIR in zip(ARCHS, DESTDIRS):
        DEST_PREFIX = path.normpath(DESTDIR + PREFIX)

        LIB = path.join(DEST_PREFIX, 'lib', 'libtro.a')

        LIPO_ARGS.append('-arch')
        if ARCH == 'aarch64':
            LIPO_ARGS.append('arm64')
        else:
            LIPO_ARGS.append(ARCH)
        LIPO_ARGS.append(LIB)

    exit_status += run_proc(LIPO, LIPO_ARGS)

    if exit_status != 0:
        return exit_status

    for DESTDIR in DESTDIRS:
        shutil.rmtree(DESTDIR, ignore_errors=True)

    BINDIR = path.join(COMBO_PREFIX, 'bin')
    Path(BINDIR).mkdir(parents=True, exist_ok=True)

    TROVSCODE_SRC = path.join(DELIVER_DIR, 'tro-vscode')
    TROVSCODE_DEST = path.join(BINDIR, 'tro-vscode')
    shutil.copyfile(TROVSCODE_SRC, TROVSCODE_DEST)
    os.chmod(TROVSCODE_DEST, mode=0o777)

    PKGBUILD = shutil.which('pkgbuild')
    if PKGBUILD == None:
        raise Exception('pkgbuild não encontrado')
    
    exit_status += run_proc(PKGBUILD, [
        '--root', INSTALL_COMBO,
        '--identifier', 'io.github.n-loco.libtro',
        '--version', VERSION,
        path.join(DIST_DIR, f'libtro_{VERSION}_apple-darwin.pkg')
    ])

    if exit_status != 0:
        return exit_status
    
    shutil.rmtree(INSTALL_COMBO, ignore_errors=True)

    return 0


if __name__ == '__main__':
    if len(sys.argv) < 2:
        raise Exception('argumentos insuficientes')

    match sys.argv[1]:
        case 'linux-gnu':
            exit(linux_gnu())
        case 'pc-windows':
            exit(pc_windows())
        case 'apple-darwin':
            exit(apple_darwin())
        case _:
            raise Exception(f'target desconhecido: {sys.argv[1]}')
