@echo off
echo Building DMD API DLL...

:: Create bin directory if it doesn't exist
if not exist bin mkdir bin

gcc -shared -o bin\dmd_api.dll ^
    src\dmd\dmd_api.c ^
    src\dmd\dmd_connection.c ^
    src\dmd\dmd_status.c ^
    src\dmd\dmd_pattern.c ^
    src\dmd\dmd_image.c ^
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

g++ -shared -o bin\asi_api.dll src\asi\asi_api.cpp -lcomdlg32 ^
    -Isrc\asi ^
    -Lsrc\asi ^
    -Ilib ^
    -Llib ^
    -lASICamera2

if %ERRORLEVEL% EQU 0 (
    echo Build successful: bin\asi_api.dll
) else (
    echo Build failed: bin\asi_api.dll
)