import os.path as path
from os import getenv, environ
from pathlib import Path
import shutil
import sys

from vsutil import VS_INSTALATION_PATH, VC_HOST_ARCH
from msysutil import MSYS_INSTALATION_PATH

__clang_format = getenv('CLANG_FORMAT_PATH')

if __clang_format == '':
    __clang_format = shutil.which('clang-format')

if sys.platform == 'win32':
    if __clang_format == None and VS_INSTALATION_PATH != '':
        VC_CLANG_FORMAT = path.join(
            VS_INSTALATION_PATH,
            'VC', 'Tools', 'Llvm',
            VC_HOST_ARCH.value,
            'bin', 'clang-format.EXE',
        )
        if Path(VC_CLANG_FORMAT).is_file():
            __clang_format = VC_CLANG_FORMAT

    if __clang_format == None and MSYS_INSTALATION_PATH != '':
        MSYS_CLANG_FORMAT = path.join(
            MSYS_INSTALATION_PATH,
            'usr', 'bin', 'clang-format.EXE'
        )
        if Path(MSYS_CLANG_FORMAT).is_file():
            __clang_format = MSYS_CLANG_FORMAT


ENVIRON: dict[str, str] = dict()
ENVIRON.update(environ)

JUST: str = shutil.which('just')
CLANG_FORMAT: str | None = __clang_format
MESON: str = getenv('MESON_PATH')
if MESON == None:
    MESON = shutil.which('meson')
if MESON == None:
    raise Exception('Meson não encontrado')

MESON_BUILDDIR: str = getenv('MESON_BUILDDIR')
if MESON_BUILDDIR == None or len(MESON_BUILDDIR) == 0:
    raise Exception('MESON_BUILDDIR não está definido')
MESON_BUILDDIR = path.abspath(path.normpath(MESON_BUILDDIR))

MESON_CROSS_DIR: str = getenv('MESON_CROSS_DIR')
if MESON_CROSS_DIR == None or len(MESON_CROSS_DIR) == 0:
    raise Exception('MESON_CROSS_DIR não está definido')
MESON_CROSS_DIR = path.abspath(path.normpath(MESON_CROSS_DIR))

DELIVER_DIR = path.abspath(path.join(path.normpath(__file__), '..', '..', 'deliver'))
