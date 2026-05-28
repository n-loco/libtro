from subprocess import Popen, PIPE

__GIT_PROC = Popen(['git', 'describe', '--always'], stdout=PIPE)
__GIT_STDOUT, _ = __GIT_PROC.communicate()
if __GIT_PROC.returncode != 0:
    exit(__GIT_PROC.returncode)

VERSION = __GIT_STDOUT.decode().strip()

if VERSION.startswith('v'):
    VERSION = VERSION[1:]
