from os import getenv, environ
import os.path as path
from subprocess import Popen, PIPE
from pathlib import Path
import sys

MSYSTEM = ''
MSYS_INSTALATION_PATH = ''
MSYS_ENV: dict[str, str] = dict()

def __load():
    if sys.platform != 'win32':
        return

    global MSYS_INSTALATION_PATH
    MSYS_INSTALATION_PATH = getenv('MSYS_PATH', 'C:\\msys64')
    if not Path(MSYS_INSTALATION_PATH).is_dir():
        MSYS_INSTALATION_PATH = ''
        return

    global MSYSTEM
    MSYSTEM = 'UCRT64'

    SCRAP_ENV = dict()
    SCRAP_ENV.update(environ)
    SCRAP_ENV.update({
        'MSYSTEM': MSYSTEM,
        'MSYS2_PATH_TYPE': 'inherit',
    })

    BASH = path.join(MSYS_INSTALATION_PATH, 'usr', 'bin', 'bash.EXE')
    BASH_SCRAP_CODE = 'env; echo PATH=$(cygpath -wp $PATH)'
    BASH_SCRAP = f'{BASH} -lc "{BASH_SCRAP_CODE}"'

    BASH_PROC = Popen(BASH_SCRAP, stdout=PIPE, env=SCRAP_ENV)
    BASH_STDOUT, _ = BASH_PROC.communicate()
    if BASH_PROC.returncode != 0:
        raise Exception(f'Bash do MSYS terminou com código de erro {BASH_PROC.returncode}')

    for LINE in BASH_STDOUT.decode().splitlines():
        if '=' in LINE:
            K, V = LINE.split('=')
            MSYS_ENV[K] = V


__load()
