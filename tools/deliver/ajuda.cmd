@ECHO OFF
SETLOCAL

: Deixando o console em UTF-8.
CHCP 65001 > NUL

SET ARCH=unknown
SET CPU=unknown

IF "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
    SET ARCH=x86_64
    SET CPU=x86_64
)
IF "%PROCESSOR_ARCHITECTURE%" == "x86" (
    SET ARCH=x86
    SET CPU=i686
)
IF "%PROCESSOR_ARCHITECTURE%" == "ARM64" (
    SET ARCH=aarch64
    SET CPU=aarch64
)

IF "%ARCH%" == "unknown" (
    ECHO Não foi possível lhe ajudar.
    ECHO Arquitetura de processador não identificada.
    GOTO EXIT_ERROR
)

ECHO Foi detectado um ambiente %ARCH%.
ECHO.

SET MSVC_TRIPLE=%ARCH%-pc-windows-msvc
SET GNU_TRIPLE=%CPU%-pc-windows-gnu

ECHO Caminho dos cabeçalhos (.h):
ECHO - %~dp0include
ECHO.

ECHO Caminho da biblioteca para MSVC compilada (*.lib):
ECHO - %~dp0lib\%MSVC_TRIPLE%
ECHO.

ECHO Caminho da biblioteca para GNU (MinGW) compilada (lib*.a):
ECHO - %~dp0lib\%GNU_TRIPLE%
ECHO.

ECHO Nome da biblioteca:
ECHO - tro
ECHO.

ECHO Recomendamos colocar isto no Path do Windows:
ECHO - %~dp0bin
ECHO.

ENDLOCAL
ECHO.
PAUSE
EXIT /B 0

:EXIT_ERROR
ENDLOCAL
ECHO.
PAUSE
EXIT /B 1
