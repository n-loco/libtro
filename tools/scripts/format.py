import os.path as path
from subprocess import Popen

from justenv import CLANG_FORMAT, MESON, JUST
from term import BOLD, ITALLIC, BRIGHT_YELLOW, BRIGHT_RED, RESET
from globr import globr

def run_tool(toolexe: str, args: list[str]) -> int:
    TOOLNAME = path.basename(toolexe)

    ARG_LINE: str
    if len(args) > 4:
        ARG_LINE = ' '.join(args[:4]) + f' ...{ITALLIC}+{len(args) - 4}{RESET}'
    else:
        ARG_LINE = ' '.join(args)

    print(f'{TOOLNAME} {ARG_LINE}')
    exit_code = Popen([toolexe, *args]).wait()
    if exit_code != 0:
        print(f'{BRIGHT_RED}{BOLD}ERRO:{RESET} {TOOLNAME} terminou com código de saída: {exit_code}')
    return exit_code

if __name__ == '__main__':
    print(f'Usando {JUST}')

    if CLANG_FORMAT == None:
        print(f'{BRIGHT_YELLOW}{BOLD}ATENÇÃO:{RESET} Clang Format não encontrado')
    else:
        print(f'Usando {CLANG_FORMAT}')

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
    if CLANG_FORMAT != None:
        exit_code += run_tool(CLANG_FORMAT, ['-i', *c_files])
    exit_code += run_tool(MESON, ['fmt', '-i', *meson_files])

    exit(exit_code)
