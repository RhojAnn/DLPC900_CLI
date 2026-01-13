@echo off
echo Building ASI Camera View...

g++ -o asi_live_view.exe asi_live_view.cpp -lcomdlg32 ^
    -Iasi ^
    -IOpenCV-MinGW-Build-OpenCV-4.5.5-x64/include ^
    -Lasi ^
    -LOpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib ^
    -lASICamera2 ^
    -lopencv_core455 -lopencv_highgui455 -lopencv_imgproc455 -lopencv_imgcodecs455

if %ERRORLEVEL% EQU 0 (
    echo Build successful: asi_live_view.exe
    echo.
    echo Running asi_live_view.exe...
    asi_live_view.exe
) else (
    echo Build failed: asi_live_view.exe
)