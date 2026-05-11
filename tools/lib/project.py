from os import getenv
import os.path as path
from enum import Enum
import json

MESON_BUILDDIR = getenv('MESON_BUILDDIR')
if MESON_BUILDDIR == None or len(MESON_BUILDDIR) == 0:
    raise Exception('MESON_BUILDDIR is undefined')
MESON_BUILDDIR = path.abspath(path.normpath(MESON_BUILDDIR))

__MESON_TARGETS_FILE_PATH = path.join(MESON_BUILDDIR, 'meson-info', 'intro-targets.json')

with open(__MESON_TARGETS_FILE_PATH) as __MESON_TARGETS_FILE:
    __MESON_TARGETS_DATA: list[dict] = json.load(__MESON_TARGETS_FILE)

class MesonTargetType(Enum):
    EXECUTABLE = 1
    STATIC_LIBRARY = 2
    SHARED_LIBRARY = 3
    SHARED_MODULE = 4
    CUSTOM = 5
    RUN = 6
    JAR = 7

    def __str__(self):
        match self:
            case MesonTargetType.EXECUTABLE:
                return 'executable'
            case MesonTargetType.STATIC_LIBRARY:
                return 'static library'
            case MesonTargetType.SHARED_LIBRARY:
                return 'shared library'
            case MesonTargetType.SHARED_MODULE:
                return 'shared module'
            case MesonTargetType.CUSTOM:
                return 'custom'
            case MesonTargetType.RUN:
                return 'run'
            case MesonTargetType.JAR:
                return 'jar'
            case _:
                return f'invalid {MesonTargetType._name_} state'

class MesonTarget:
    __name: str
    __filename: list[str]
    __type: MesonTargetType

    @property
    def name(self):
        return self.__name

    @property
    def filename(self):
        return self.__filename

    @property
    def type(self):
        return self.__type

    def __init__(self, src: dict):
        self.__name = src['name']
        self.__filename = src['filename']
        match src['type']:
            case 'executable':
                self.__type = MesonTargetType.EXECUTABLE
            case 'static library':
                self.__type = MesonTargetType.STATIC_LIBRARY
            case 'shared library':
                self.__type = MesonTargetType.SHARED_LIBRARY
            case 'shared module':
                self.__type = MesonTargetType.SHARED_MODULE
            case 'custom':
                self.__type = MesonTargetType.CUSTOM
            case 'run':
                self.__type = MesonTargetType.RUN
            case 'jar':
                self.__type = MesonTargetType.JAR
            case _:
                raise Exception(f'Unknown target type "{src["type"]}"')

    def islibrary(self) -> bool:
        match self.__type:
            case MesonTargetType.STATIC_LIBRARY:
                return True
            case MesonTargetType.SHARED_LIBRARY:
                return True
            case MesonTargetType.SHARED_MODULE:
                return True
            case _:
                return False

MESON_TARGETS: list[MesonTarget] = []

for __DATA in __MESON_TARGETS_DATA:
    MESON_TARGETS.append(MesonTarget(__DATA))
