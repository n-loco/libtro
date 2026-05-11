from os import getenv
import os.path as path
from subprocess import Popen, PIPE

from globr import globr

def run_tool(toolexe: str, args: list[str]) -> int:
    TOOLNAME = path.basename(toolexe)
    print(f'Running {TOOLNAME}')
    proc = Popen([toolexe, *args])
    return proc.wait()

if __name__ == '__main__':
    exit_code = 0

    JUST = 'just'
    CLANG_FORMAT = getenv('CLANG_FORMAT_PATH', 'clang-format')
    MESON = getenv('MESON_PATH', 'meson')

    print(f'Using {CLANG_FORMAT}')
    print(f'Using {MESON}')

    print()

    c_files = globr([
        'src/**/*.c',
        'src/**/*.h',
        'tests/**/*.c',
        'tests/**/*.h',
        'include/**/*.h',
    ])

    meson_files = globr([
        'meson.build',
        'src/**/meson.build',
        'tests/**/meson.build',
        'include/**/meson.build',
    ])

    exit_code = run_tool(JUST, ['--fmt'])
    exit_code = run_tool(CLANG_FORMAT, ['-i', *c_files])
    exit_code = run_tool(MESON, ['fmt', '-i', *meson_files])

    exit(exit_code)
