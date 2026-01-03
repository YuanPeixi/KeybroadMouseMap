@echo off
REM Build script for Windows

echo Building KeyboardMouseMap...
echo.

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Enter build directory
cd build

REM Generate Visual Studio project files
echo Generating build files with CMake...
cmake ..

if %errorlevel% neq 0 (
    echo.
    echo ERROR: CMake configuration failed!
    echo Make sure CMake is installed and in your PATH.
    pause
    exit /b %errorlevel%
)

echo.
echo Building project...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Build failed!
    pause
    exit /b %errorlevel%
)

echo.
echo ======================================
echo Build completed successfully!
echo ======================================
echo.
echo Executable location: build\bin\Release\KeyboardMouseMap.exe
echo.
echo To run the application:
echo   cd build\bin\Release
echo   KeyboardMouseMap.exe
echo.
pause
