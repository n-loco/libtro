import os.path as path
import json

from mespy import MesonTarget
from justenv import MESON_BUILDDIR

__MESON_TARGETS_FILE_PATH = path.join(MESON_BUILDDIR, 'meson-info', 'intro-targets.json')

with open(__MESON_TARGETS_FILE_PATH) as __MESON_TARGETS_FILE:
    __MESON_TARGETS_DATA: list[dict] = json.load(__MESON_TARGETS_FILE)

MESON_TARGETS: list[MesonTarget] = []

for __DATA in __MESON_TARGETS_DATA:
    MESON_TARGETS.append(MesonTarget(__DATA))
