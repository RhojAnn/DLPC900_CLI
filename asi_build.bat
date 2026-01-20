@echo off
echo Building ASI Camera View...

:: Create bin directory if it doesn't exist
if not exist bin mkdir bin

g++ -o bin\asi_view.dll asi\asi_view.cpp -lcomdlg32 ^
    -Iasi ^
    -IOpenCV-MinGW-Build-OpenCV-4.5.5-x64/include ^
    -Lasi ^
    -LOpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib ^
    -lASICamera2 ^
    -lopencv_core455 -lopencv_highgui455 -lopencv_imgproc455 -lopencv_imgcodecs455

copy /Y lib\asi\*.dll bin\

if %ERRORLEVEL% EQU 0 (
    echo Build successful: bin\asi_view.exe
    echo.
    echo Running bin\asi_view.exe...
    bin\asi_view.exe
) else (
    echo Build failed: bin\asi_view.exe
)