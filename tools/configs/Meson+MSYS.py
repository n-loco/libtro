import os.path as path

from justenv import MESON_CROSS_DIR
from msysutil import MSYS_INSTALATION_PATH

CROSS_TEMPLATE = '''; Cross-file gerado por: just config Meson+MSYS
; ATENÇÃO: Apenas o cross-file não é o suficiente, o ambiente MSYS UCRT64 precisa estar carregado.
; Este cross-file assume os pacotes mingw-w64-cross-clang e lld instalados

[constants]
mingw_triple = '{arch}-w64-mingw32'
msys_install_path = {msys_install_path}
sysroot = msys_install_path / 'opt' / mingw_triple

[binaries]
c = 'clang'
ar = 'llvm-ar'
c_ld = 'lld'
strip = 'llvm-strip'

[built-in options]
c_args = ['--start-no-unused-arguments', '-target', mingw_triple, '-rtlib=compiler-rt', '--sysroot=' + sysroot, '--end-no-unused-arguments']
c_link_args = c_args
libdir = {libdir}
pkgconfig.relocatable = true

[host_machine]
system = 'windows'
cpu_family = '{cpu_family}'
cpu = '{arch}'
endian = 'little'
'''

if __name__ == '__main__':
    ARCHS = [
        'x86_64',
        'i686',
        'aarch64',
    ]

    MSYS_INSTALL_PATH = ' / '.join(map(lambda s: f"'{s}'", MSYS_INSTALATION_PATH.split('\\')))

    for ARCH in ARCHS:
        TARGET_TRIPLE = f'{ARCH}-pc-windows-gnu'

        LIBDIR = f"'lib' / '{TARGET_TRIPLE}'"

        if ARCH == 'i686':
            CPU_FAMILY = 'x86'
        else:
            CPU_FAMILY = ARCH

        CONTENT = CROSS_TEMPLATE.format(
            arch=ARCH,
            mingw_triple=f'{ARCH}-w64-mingw32',
            msys_install_path=MSYS_INSTALL_PATH,
            libdir=LIBDIR,
            cpu_family=CPU_FAMILY
        )

        CROSS_FILE_PATH = path.join(MESON_CROSS_DIR, TARGET_TRIPLE + '.ini')
        with open(CROSS_FILE_PATH, 'wb') as CROSS_FILE:
            CROSS_FILE.write(bytes(CONTENT, encoding='utf8'))

