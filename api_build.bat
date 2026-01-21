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

g++ -shared -o bin\asi_api.dll asi\asi_api.cpp -lcomdlg32 ^
    -Iasi ^
    -IOpenCV-MinGW-Build-OpenCV-4.5.5-x64/include ^
    -Lasi ^
    -LOpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib ^
    -lASICamera2 ^
    -lopencv_core455 -lopencv_highgui455 -lopencv_imgproc455 -lopencv_imgcodecs455

copy /Y lib\asi\*.dll bin\

if %ERRORLEVEL% EQU 0 (
    echo Build successful: bin\asi_api.dll
) else (
    echo Build failed: bin\asi_api.dll
)