@echo off
echo Building DLPC900 CLI...

:: Create bin directory if it doesn't exist
if not exist bin mkdir bin

gcc -o bin\dlpc900_cli.exe ^
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
    bin/dlpc900_cli.exe
) else (
    echo Build failed!
)

:: Build the OpenCV image viewer utility
:: g++ -o show_image.exe show_image.cpp ^
::    -IOpenCV-MinGW-Build-OpenCV-4.5.5-x64/include ^
::    -LOpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib ^
::    -lopencv_core455 -lopencv_highgui455 -lopencv_imgproc455 -lopencv_imgcodecs455
::
:: if %ERRORLEVEL% EQU 0 (
::    echo Build successful: show_image.exe
::    show_image.exe OpenCV_test.jpg
::) else (
::    echo Build failed: show_image.exe
::)