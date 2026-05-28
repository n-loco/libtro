@ECHO OFF
SETLOCAL EnableDelayedExpansion
(set NL=^

)

IF NOT EXIST .vscode (
    MKDIR .vscode
)

SET MACHINE=unknown

IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
    SET MACHINE=x86_64
)
IF "%PROCESSOR_ARCHITECTURE%" == "x86" (
    SET MACHINE=i686
)
IF "%PROCESSOR_ARCHITECTURE%" == "ARM64" (
    SET MACHINE=aarch64
)

SET GNU_TRIPLE=%MACHINE%-pc-windows-gnu

SET LIBDIR_RAW=%~dp0..\lib\%GNU_TRIPLE%
FOR %%i IN ("%LIBDIR_RAW%") DO SET "LIBDIR=%%~fi"
SET LIBDIR_ESC=%LIBDIR:\=\\%

SET INCLUDEDIR_RAW=%~dp0..\include
FOR %%i IN ("%INCLUDEDIR_RAW%") DO SET "INCLUDEDIR=%%~fi"
SET INCLUDEDIR_ESC=%INCLUDEDIR:\=\\%

SET TASKS_JSON={!NL!^
    "version": "2.0.0",!NL!^
    "tasks": [!NL!^
        {!NL!^
            "label": "createdir",!NL!^
            "type": "shell",!NL!^
            "command": "mkdir",!NL!^
            "args": ["-Force", "${fileDirname}\\build"],!NL!^
            "hide": true,!NL!^
            "presentation": {!NL!^
                "revealProblems": "onProblem",!NL!^
                "echo": true,!NL!^
                "reveal": "never",!NL!^
                "focus": false,!NL!^
                "panel": "shared"!NL!^
            }!NL!^
        },!NL!^
        {!NL!^
            "label": "Compilar Arquivo Atual",!NL!^
            "type": "process",!NL!^
            "dependsOn": ["createdir"],!NL!^
            "command": "cc",!NL!^
            "args": [!NL!^
                "-g",!NL!^
                "-o",!NL!^
                "${fileDirname}\\build\\${fileBasenameNoExtension}.exe",!NL!^
                "-I%INCLUDEDIR_ESC%",!NL!^
                "${file}",!NL!^
                "-L%LIBDIR_ESC%",!NL!^
                "-ltro"!NL!^
            ],!NL!^
            "isBuildCommand": true,!NL!^
            "problemMatcher": "$nvcc",!NL!^
            "icon": {!NL!^
                "color": "terminal.ansiGreen",!NL!^
                "id": "build"!NL!^
            },!NL!^
            "presentation": {!NL!^
                "revealProblems": "onProblem",!NL!^
                "echo": true,!NL!^
                "reveal": "silent",!NL!^
                "focus": false,!NL!^
                "panel": "shared",!NL!^
                "close": true!NL!^
            }!NL!^
        }!NL!^
    ]!NL!^
}

SET LAUNCH_JSON={!NL!^
    "version": "0.2.0",!NL!^
    "configurations": [!NL!^
        {!NL!^
            "name": "Rodar Arquivo Atual",!NL!^
            "type": "lldb",!NL!^
            "request": "launch",!NL!^
            "program": "${fileDirname}\\build\\${fileBasenameNoExtension}.exe",!NL!^
            "args": [],!NL!^
            "cwd": "${workspaceRoot}",!NL!^
            "preLaunchTask": "Compilar Arquivo Atual"!NL!^
        }!NL!^
    ]!NL!^
}

SET CLANGD=^
CompileFlags:!NL!^
  Add:!NL!^
    - -I%INCLUDEDIR%

ECHO !TASKS_JSON! > .vscode\tasks.json
ECHO !LAUNCH_JSON! > .vscode\launch.json
ECHO !CLANGD! > .clangd

ENDLOCAL
EXIT /B
