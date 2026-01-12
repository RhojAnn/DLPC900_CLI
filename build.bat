@echo off
echo Building DLPC900 CLI...

gcc -o dlpc900_cli.exe ^
    src\main.c ^
    src\cmd_status.c ^
    src\cmd_pattern.c ^
    src\cmd_image.c ^
    src\cmd_connection.c ^
    lib\diagnosticFile.c ^
    lib\API.c ^
    lib\usb.c ^
    lib\pattern.c ^
    lib\splash.c ^
    lib\compress.c ^
    lib\BMPParser.c ^
    lib\Error.c ^
    hidapi\hid.c ^
    -Ilib -Ihidapi -Isrc ^
    -lsetupapi -lhid ^
    -DWIN64 ^
    -Wall -O2

if %ERRORLEVEL% EQU 0 (
    echo Build successful: dlpc900_cli.exe
    echo.
    echo Running dlpc900_cli.exe...
    dlpc900_cli.exe
) else (
    echo Build failed!
)