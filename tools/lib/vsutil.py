from enum import Enum
from subprocess import Popen, PIPE
from os import getenv
from typing import Self
import os.path as path
import json
import sys

class VCArch(Enum):
    UNKNOWN = ''
    X86 = 'x86'
    X64 = 'x64'
    ARM64 = 'arm64'

    def cross_str(self, arch: Self) -> str:
        if self.value == arch.value:
            return self.value

        return f'{self.value}_{arch.value}'


    def normalized(self) -> str:
        match self:
            case self.X86:
                return 'x86'
            case self.X64:
                return 'x86_64'
            case self.ARM64:
                return 'aarch64'
            case _:
                return 'unknown'


VS_MAJOR_VERSION = 0
VS_INSTALATION_PATH = ''
VC_HOST_ARCH = VCArch.UNKNOWN

__VCVARS = ''

__vcvars_cache: dict[str, dict[str, str]] = dict()

def vcvars(arch: VCArch) -> dict[str, str]:
    if __VCVARS == '':
        raise Exception('vsutil não carregado corretamente')

    CACHE = __vcvars_cache.get(arch.value)
    if CACHE != None:
        return CACHE

    CMD_SCRAP_CODE = fr'"{__VCVARS}" {VC_HOST_ARCH.cross_str(arch)} && SET'
    CMD = Popen(f'CMD /C {CMD_SCRAP_CODE}', stdout=PIPE)
    CMD_STDOUT, _ = CMD.communicate()
    if CMD.returncode != 0:
        raise Exception(f'CMD retornou com {CMD.returncode}')

    env = dict()
    for LINE in CMD_STDOUT.decode().splitlines():
        if '=' in LINE:
            K, V = LINE.split('=')
            env[K] = V

    __vcvars_cache[arch.value] = env
    return env


def __load():
    if sys.platform != 'win32':
        return

    global VC_HOST_ARCH
    match getenv('PROCESSOR_ARCHITECTURE', '').lower():
        case 'amd64':
            VC_HOST_ARCH = VCArch.X64
        case 'x86':
            VC_HOST_ARCH = VCArch.X86
        case 'arm64':
            VC_HOST_ARCH = VCArch.ARM64
        case _:
            return

    VSWHERE_HARDPATH = path.join(
        getenv('ProgramFiles(x86)'),
        'Microsoft Visual Studio',
        'Installer',
        'vswhere.EXE',
    )
    VSWHERE = getenv('VSWHERE', VSWHERE_HARDPATH)

    global VS_MAJOR_VERSION
    VS_MAJOR_VERSION = 17

    VERSION_RANGE = f'[{VS_MAJOR_VERSION}.0,{VS_MAJOR_VERSION + 1}.0)'

    VSWHERE_PROC = Popen([
        VSWHERE,
        '-version', VERSION_RANGE,
        '-latest',
        '-utf8',
        '-format', 'json',
    ], stdout=PIPE)
    VSWHERE_STDOUT, _ = VSWHERE_PROC.communicate()
    VSWHERE_DATA = json.loads(VSWHERE_STDOUT.decode())[0]

    global VS_INSTALATION_PATH
    VS_INSTALATION_PATH = VSWHERE_DATA['resolvedInstallationPath']

    global __VCVARS
    __VCVARS = path.join(VS_INSTALATION_PATH, 'VC', 'Auxiliary', 'Build', 'vcvarsall.bat')


__load()
