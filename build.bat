@echo off
echo Building DLPC900 CLI...

gcc -o dlpc900_cli.exe ^
    src\main.c ^
    lib\diagnosticFile.c ^
    lib\API.c ^
    lib\usb.c ^
    lib\pattern.c ^
    lib\splash.c ^
    lib\compress.c ^
    lib\BMPParser.c ^
    lib\Error.c ^
    hidapi\hid.c ^
    -Ilib -Ihidapi ^
    -lsetupapi -lhid ^
    -DWIN32 ^
    -Wall -O2

if %ERRORLEVEL% EQU 0 (
    echo Build successful: dlpc900_cli.exe
) else (
    echo Build failed!
)