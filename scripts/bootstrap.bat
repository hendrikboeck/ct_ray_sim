@echo off
REM Bootstrap script for setting up the project on Windows using Batch

REM Function to check for required tools
:CheckDependencies
SET dependencies=curl git cmake make
FOR %%G IN (%dependencies%) DO (
    where %%G >nul 2>&1
    IF ERRORLEVEL 1 (
        echo Error: '%%G' is not installed. Please install it and retry.
        EXIT /B 1
    )
)
GOTO :EOF

REM Function to set up using vcpkg
:SetupVcpkg
echo Setting up using vcpkg...

REM Copy necessary files
copy /Y "scripts\resources\vcpkg\CMakeLists.txt" "."
copy /Y "scripts\resources\vcpkg\c_cpp_properties.json" ".vscode\"

REM Clone vcpkg repository if it doesn't exist
IF NOT EXIST "vcpkg" (
    echo Cloning vcpkg repository...
    git clone --depth=1 https://github.com/microsoft/vcpkg.git
) ELSE (
    echo vcpkg directory already exists. Skipping clone.
)

REM Bootstrap vcpkg
IF EXIST "vcpkg\bootstrap-vcpkg.bat" (
    echo Bootstrapping vcpkg...
    call vcpkg\bootstrap-vcpkg.bat
) ELSE (
    echo Error: bootstrap-vcpkg.bat not found.
    EXIT /B 1
)

REM Install required packages
echo Installing required packages...
call vcpkg\vcpkg.exe install opencv[core,imgproc,imgcodecs] glm spdlog fmt argparse

REM Set up CMake integration with vcpkg
echo Configuring CMake to use vcpkg toolchain file...

REM Define the path to the vcpkg toolchain file
SET "VCPKG_TOOLCHAIN_FILE=%CD%\vcpkg\scripts\buildsystems\vcpkg.cmake"

REM Check if the toolchain file exists
IF NOT EXIST "%VCPKG_TOOLCHAIN_FILE%" (
    echo Error: vcpkg toolchain file not found at %VCPKG_TOOLCHAIN_FILE%.
    EXIT /B 1
)

REM Create or update CMakeSettings.json for VSCode
(
echo {
echo     "configurations": [
echo         {
echo             "name": "x64-Debug",
echo             "generator": "Ninja",
echo             "configurationType": "Debug",
echo             "buildRoot": "\${projectDir}/build/debug",
echo             "installRoot": "\${projectDir}/build/install",
echo             "cmakeCommandArgs": "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_TOOLCHAIN_FILE%",
echo             "buildCommandArgs": "",
echo             "ctestCommandArgs": "",
echo             "inheritEnvironments": [ "msvc_x64" ],
echo             "variables": []
echo         },
echo         {
echo             "name": "x64-Release",
echo             "generator": "Ninja",
echo             "configurationType": "Release",
echo             "buildRoot": "\${projectDir}/build/release",
echo             "installRoot": "\${projectDir}/build/install",
echo             "cmakeCommandArgs": "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_TOOLCHAIN_FILE%",
echo             "buildCommandArgs": "",
echo             "ctestCommandArgs": "",
echo             "inheritEnvironments": [ "msvc_x64" ],
echo             "variables": []
echo         }
echo     ]
echo }
) > ".vscode\CMakeSettings.json"

echo CMakeSettings.json has been configured to use the vcpkg toolchain file.
echo If you are using VSCode, consider reloading the window to apply the new CMake settings.

echo Setup complete. You can now build the project using CMake.
echo 1. Include .env: include .env
echo 2. Run cmake: cmake -B build -DCMAKE_BUILD_TYPE=Release --toolchain %VCPKG_TOOLCHAIN_FILE% .
echo 3. Run make: cmake --build build --config Release
echo 4. Run the executable: .\build\ct_ray_sim.exe --inputPath .\input.png --angles 32
GOTO :EOF

REM Function to set up from source
:SetupFromSource
echo Setting up from source...

REM Check if 'build' directory does not exist
IF NOT EXIST "build" (
    echo build directory does not exist. Checking connectivity to github.com...
    curl -I https://github.com/ --max-time 5 >nul 2>&1
    IF ERRORLEVEL 1 (
        echo Error: Unable to reach github.com. Please check your internet connection.
        EXIT /B 1
    ) ELSE (
        echo Successfully connected to github.com.
    )
) ELSE (
    echo build directory already exists. Skipping connectivity check.
)

REM Proceed with setup
copy /Y "scripts\resources\fromSource\CMakeLists.txt" "."
copy /Y "scripts\resources\fromSource\c_cpp_properties.json" ".vscode\"

REM Optional: Create build directory if it doesn't exist
IF NOT EXIST "build" (
    mkdir build
    echo Created 'build' directory.
)

REM Navigate to build directory
cd build

REM Check if CMakeLists.txt exists in the parent directory
IF NOT EXIST "..\CMakeLists.txt" (
    echo Error: CMakeLists.txt not found in the parent directory.
    EXIT /B 1
)

echo Building project using CMake:
echo 1. Run cmake: cmake -B build -DCMAKE_BUILD_TYPE=Release ..
echo 2. Run make: cmake --build build --config Release
echo 3. Run the executable: .\build\ct_ray_sim.exe --inputPath .\input.png --angles 32
GOTO :EOF

REM Function to display help message
:ShowHelp
echo Usage: bootstrap.bat [Option]
echo.
echo Options:
echo   -vcpkg        Set up using the vcpkg package manager
echo   -fromSource   Set up using CMake from source
echo   -h, -help     Show this help message
GOTO :EOF

REM Function to handle invalid options
:InvalidOption
echo Invalid option: %1
echo Use -h or -help to see available options.
EXIT /B 1

REM Main script logic
CALL :CheckDependencies

IF "%~1"=="" (
    CALL :ShowHelp
    EXIT /B 0
)

SET "option=%~1"

IF /I "%option%"=="-vcpkg" (
    CALL :SetupVcpkg
    EXIT /B 0
) ELSE IF /I "%option%"=="-fromSource" (
    CALL :SetupFromSource
    EXIT /B 0
) ELSE IF /I "%option%"=="-h" (
    CALL :ShowHelp
    EXIT /B 0
) ELSE IF /I "%option%"=="-help" (
    CALL :ShowHelp
    EXIT /B 0
) ELSE (
    CALL :InvalidOption %option%
)
