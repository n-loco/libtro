from os import getenv
import os.path as path
from subprocess import Popen
from term import BOLD, ITALLIC, YELLOW, RESET

from globr import globr

def run_tool(toolexe: str, args: list[str]) -> int:
    TOOLNAME = path.basename(toolexe)

    argline: str
    if len(args) > 6:
        argline = ' '.join(args[:4]) + f' ...{ITALLIC}+{len(args) - 6}{RESET}'
    else:
        argline = ' '.join(args)

    print(f'{TOOLNAME} {argline}')

    proc = Popen([toolexe, *args])
    return proc.wait()

if __name__ == '__main__':
    exit_code = 0

    JUST = 'just'
    CLANG_FORMAT = getenv('CLANG_FORMAT_PATH', 'clang-format')
    MESON = getenv('MESON_PATH', 'meson')

    if CLANG_FORMAT != '':
        print(f'Usando {CLANG_FORMAT}')
    else:
        print(f'{YELLOW}{BOLD}ATENÇÃO:{RESET} Clang Format não encontrado')

    print(f'Usando {MESON}')

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

    exit_code = 0

    exit_code += run_tool(JUST, ['--fmt'])
    if CLANG_FORMAT != '':
        exit_code += run_tool(CLANG_FORMAT, ['-i', *c_files])
    exit_code += run_tool(MESON, ['fmt', '-i', *meson_files])

    exit(exit_code)
