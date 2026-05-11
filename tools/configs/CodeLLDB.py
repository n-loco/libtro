from project import MESON_BUILDDIR, MESON_TARGETS, MesonTargetType
import os.path as path
import posixpath
import json

launch_data = {
    'version': '0.2.0',
    'configurations': [],
}

def append_launch(name: str, program: str):
    program = posixpath.normpath(program)

    TASK_PATH = program.replace(posixpath.normpath(MESON_BUILDDIR)+'/', '').replace('.exe', '')
    TASK = f'Meson: Build {TASK_PATH}:executable'

    launch_data['configurations'].append({
        'name': name,
        'type': 'lldb',
        'request': 'launch',
        'program': program,
        'preLaunchTask': TASK,
        'args': [],
        'cwd': '${workspaceRoot}',
    })

if __name__ == '__main__':
    for TARGET in MESON_TARGETS:
        if TARGET.type == MesonTargetType.EXECUTABLE:
            append_launch(TARGET.name, TARGET.filename[0])

    with open(path.join('.vscode', 'launch.json'), 'w') as LAUNCH_FILE:
        json.dump(launch_data, LAUNCH_FILE, indent=2)

