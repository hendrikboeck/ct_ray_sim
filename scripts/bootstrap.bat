@echo off
setlocal

REM Default action if no options are provided
set "DEFAULT_OPTION=--vcpkg"

REM Check for provided options and call respective script
if "%~1"=="" (
    set "OPTION=%DEFAULT_OPTION%"
) else (
    set "OPTION=%~1"
)

if OPTION=="--help" (
    set "OPTION=-h"
)

if "%OPTION%"=="--vcpkg" (
    echo Setting up using vcpkg...
    copy "scripts\resources\vcpkg\c_cpp_properties.json" .vscode\
    copy "scripts\resources\vcpkg\CMakeLists.txt" .\
    git clone --depth=1 https://github.com/microsoft/vcpkg.git vcpkg
    cd vcpkg
    bootstrap-vcpkg.bat
    cd ..
    vcpkg\vcpkg.exe install opencv4 glm spdlog fmt argparse
    exit /b 0
) else if "%OPTION%"=="--fromSource" (
    echo Setting up from source...
    copy "scripts\resources\fromSource\CMakeLists.txt" .\
    copy "scripts\resources\fromSource\c_cpp_properties.json" .vscode\
    exit /b 0
) else if "%OPTION%"=="-h" (
    echo Usage: bootstrap.bat [OPTION]
    echo Options:
    echo   --vcpkg        Set up using the vcpkg package manager - !!requires git
    echo   --fromSource   Set up using cmake from source - !!may be slow
    echo   -h, --help     Show this help message
    exit /b 0
) else (
    echo Invalid option: %OPTION%
    echo Use -h or --help to see available options.
    exit /b 1
)
