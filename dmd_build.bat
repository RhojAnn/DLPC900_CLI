@echo off
echo Building DMD API DLL...

:: Create bin directory if it doesn't exist
if not exist bin mkdir bin

gcc -shared -o bin\dmd_api.dll ^
    src\dmd_api.c ^
    src\dmd_connection.c ^
    src\dmd_status.c ^
    src\dmd_pattern.c ^
    src\dmd_image.c ^
    lib\API.c ^
    lib\usb.c ^
    lib\pattern.c ^
    lib\splash.c ^
    lib\compress.c ^
    lib\BMPParser.c ^
    lib\Error.c ^
    lib\diagnosticFile.c ^
    hidapi\hid.c ^
    -Ilib -Ihidapi -Isrc ^
    -lsetupapi -lhid ^
    -DWIN64 ^
    -Wall -O2

if %ERRORLEVEL% EQU 0 (
    echo Build successful: bin\dmd_api.dll
) else (
    echo Build failed!
)
