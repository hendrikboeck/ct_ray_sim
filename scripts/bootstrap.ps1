#!/usr/bin/env pwsh

<#
.SYNOPSIS
    Bootstrap script for setting up the project on Windows.

.DESCRIPTION
    This script sets up the project using either vcpkg or from source.
    It checks for necessary dependencies and guides the user through the setup process.

.USAGE
    .\bootstrap.ps1 [Option]

.PARAMETER vcpkg
    Set up using the vcpkg package manager.

.PARAMETER fromSource
    Set up using CMake from source.

.PARAMETER help
    Show the help message.

.EXAMPLE
    .\bootstrap.ps1 -vcpkg

    Sets up the project using vcpkg.
#>

# Function to check for required tools
function Check-Dependencies {
    $dependencies = @("curl", "git", "cmake", "make")
    foreach ($tool in $dependencies) {
        if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
            Write-Error "Error: '$tool' is not installed. Please install it and retry."
            exit 1
        }
    }
}

# Function to set up using vcpkg
function Setup-Vcpkg {
    Write-Host "Setting up using vcpkg..." -ForegroundColor Green

    # Copy necessary files
    Copy-Item -Path "scripts\resources\vcpkg\CMakeLists.txt" -Destination "." -Force
    Copy-Item -Path "scripts\resources\vcpkg\c_cpp_properties.json" -Destination ".vscode\" -Force

    # Clone vcpkg repository if it doesn't exist
    if (-Not (Test-Path "vcpkg")) {
        Write-Host "Cloning vcpkg repository..."
        git clone --depth=1 https://github.com/microsoft/vcpkg.git
    }
    else {
        Write-Host "vcpkg directory already exists. Skipping clone." -ForegroundColor Yellow
    }

    # Bootstrap vcpkg
    $bootstrapScript = "vcpkg\bootstrap-vcpkg.bat"
    if (Test-Path $bootstrapScript) {
        Write-Host "Bootstrapping vcpkg..."
        & $bootstrapScript
    }
    else {
        Write-Error "Error: bootstrap-vcpkg.bat not found."
        exit 1
    }

    # Install required packages
    Write-Host "Installing required packages..."
    & "vcpkg\vcpkg.exe" install opencv[core,imgproc,imgcodecs] glm spdlog fmt argparse

    # Set up CMake integration with vcpkg
    Write-Host "Configuring CMake to use vcpkg toolchain file..." -ForegroundColor Green

    # Define the path to the vcpkg toolchain file
    $VCPKG_TOOLCHAIN_FILE = "$(Resolve-Path vcpkg\scripts\buildsystems\vcpkg.cmake)"

    # Check if the toolchain file exists
    if (-Not (Test-Path $VCPKG_TOOLCHAIN_FILE)) {
        Write-Error "Error: vcpkg toolchain file not found at $VCPKG_TOOLCHAIN_FILE."
        exit 1
    }

    # Create or update CMakeSettings.json for VSCode
    $CMAKE_SETTINGS_PATH = ".vscode\CMakeSettings.json"

    $cmakeSettings = @"
{
    "configurations": [
        {
            "name": "x64-Debug",
            "generator": "Ninja",
            "configurationType": "Debug",
            "buildRoot": "\${projectDir}/build/debug",
            "installRoot": "\${projectDir}/build/install",
            "cmakeCommandArgs": "-DCMAKE_TOOLCHAIN_FILE=${VCPKG_TOOLCHAIN_FILE}",
            "buildCommandArgs": "",
            "ctestCommandArgs": "",
            "inheritEnvironments": [ "msvc_x64" ],
            "variables": []
        },
        {
            "name": "x64-Release",
            "generator": "Ninja",
            "configurationType": "Release",
            "buildRoot": "\${projectDir}/build/release",
            "installRoot": "\${projectDir}/build/install",
            "cmakeCommandArgs": "-DCMAKE_TOOLCHAIN_FILE=${VCPKG_TOOLCHAIN_FILE}",
            "buildCommandArgs": "",
            "ctestCommandArgs": "",
            "inheritEnvironments": [ "msvc_x64" ],
            "variables": []
        }
    ]
}
"@

    $cmakeSettings | Out-File -FilePath $CMAKE_SETTINGS_PATH -Encoding utf8 -Force

    Write-Host "CMakeSettings.json has been configured to use the vcpkg toolchain file." -ForegroundColor Green
    Write-Host "If you are using VSCode, consider reloading the window to apply the new CMake settings." -ForegroundColor Yellow

    Write-Host "Setup complete. You can now build the project using CMake." -ForegroundColor Green
    Write-Host "1. Run cmake: cmake -B build -DCMAKE_BUILD_TYPE=Release --toolchain $VCPKG_TOOLCHAIN_FILE ."
    Write-Host "2. Run make: cmake --build build --config Release"
    Write-Host "3. Run the executable: .\build\ct_ray_sim.exe --inputPath .\input.png --angles 32"
}

# Function to set up from source
function Setup-FromSource {
    Write-Host "Setting up from source..." -ForegroundColor Green

    # Check if 'build' directory does not exist
    if (-Not (Test-Path "build")) {
        Write-Host "'build' directory does not exist. Checking connectivity to github.com..."

        # Attempt to reach github.com using curl with a timeout of 5 seconds
        try {
            $response = curl -I https://github.com/ --max-time 5 -UseBasicParsing
            Write-Host "Successfully connected to github.com." -ForegroundColor Green
        }
        catch {
            Write-Error "Error: Unable to reach github.com. Please check your internet connection."
            exit 1
        }
    }
    else {
        Write-Host "'build' directory already exists. Skipping connectivity check." -ForegroundColor Yellow
    }

    # Proceed with setup
    Copy-Item -Path "scripts\resources\fromSource\CMakeLists.txt" -Destination "." -Force
    Copy-Item -Path "scripts\resources\fromSource\c_cpp_properties.json" -Destination ".vscode\" -Force

    # Optional: Create build directory if it doesn't exist
    if (-Not (Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
        Write-Host "Created 'build' directory." -ForegroundColor Green
    }

    # Navigate to build directory
    Push-Location "build"

    # Check if CMakeLists.txt exists in the parent directory
    if (-Not (Test-Path "..\CMakeLists.txt")) {
        Write-Error "Error: CMakeLists.txt not found in the parent directory."
        Pop-Location
        exit 1
    }

    Write-Host "Building project using CMake:" -ForegroundColor Green
    Write-Host "1. Run cmake: cmake -B build -DCMAKE_BUILD_TYPE=Release .."
    Write-Host "2. Run make: cmake --build build --config Release"
    Write-Host "3. Run the executable: .\build\ct_ray_sim.exe --inputPath .\input.png --angles 32"

    Pop-Location
}

# Function to display help message
function Show-HelpMessage {
    $helpMessage = @"
Usage: .\bootstrap.ps1 [Option]

Options:
  -vcpkg        Set up using the vcpkg package manager
  -fromSource   Set up using CMake from source
  -h, -help     Show this help message
"@
    Write-Host $helpMessage
}

# Function to handle invalid options
function Invalid-Option {
    param (
        [string]$Option
    )
    Write-Error "Invalid option: $Option"
    Write-Host "Use -h or -help to see available options." -ForegroundColor Yellow
    exit 1
}

# Main script logic
function Main {
    param (
        [string[]]$Args
    )

    # Check for required dependencies first
    Check-Dependencies

    # If no arguments are provided, show help
    if ($Args.Count -eq 0) {
        Show-HelpMessage
        exit 0
    }

    # Process the first argument
    switch ($Args[0]) {
        '-vcpkg' {
            Setup-Vcpkg
            break
        }
        '-fromSource' {
            Setup-FromSource
            break
        }
        '-h' | '-help' {
            Show-HelpMessage
            break
        }
        default {
            Invalid-Option -Option $Args[0]
            break
        }
    }
}

# Invoke the main function with all passed arguments
Main -Args $args
