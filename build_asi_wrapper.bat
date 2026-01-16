@echo off
echo Building ASI Camera Wrapper DLL...

:: Create bin directory if it doesn't exist
if not exist bin mkdir bin

:: Compile the library and wrapper as a shared DLL
g++ -shared -o bin\asi_wrapper.dll ^
    asi\asi_lib.cpp ^
    asi\asi_wrapper.cpp ^
    -Iasi ^
    -IOpenCV-MinGW-Build-OpenCV-4.5.5-x64/include ^
    -Lasi ^
    -LOpenCV-MinGW-Build-OpenCV-4.5.5-x64/x64/mingw/lib ^
    -lASICamera2 ^
    -lopencv_core455 ^
    -lcomdlg32 ^
    -Wl,--out-implib,bin\libasi_wrapper.a

:: Copy required DLLs to bin directory
copy /Y lib\asi\*.dll bin\

if %ERRORLEVEL% EQU 0 (
    echo Build successful: bin\asi_wrapper.dll
    echo You can now use this DLL with Python via ctypes
) else (
    echo Build failed!
)
