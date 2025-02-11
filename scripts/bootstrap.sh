#!/usr/bin/env bash

# Default action if no options are provided
DEFAULT_OPTION="--nix"

# Function to check for required tools
check_dependencies() {
    local dependencies=(curl git cmake make)
    for tool in "${dependencies[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            echo "Error: '$tool' is not installed. Please install it and retry."
            exit 1
        fi
    done
}

# Function to set up using Nix
setup_nix() {
    echo "Setting up using nix..."
    cp scripts/resources/nix/.envrc ./
    cp scripts/resources/nix/shell.nix ./
    cp scripts/resources/nix/CMakeLists.txt ./
    direnv allow .
}

# Function to set up using vcpkg
setup_vcpkg() {
    echo "Setting up using vcpkg..."
    cp scripts/resources/vcpkg/CMakeLists.txt ./
    cp scripts/resources/vcpkg/c_cpp_properties.json .vscode/

    # Clone vcpkg repository if it doesn't exist
    if [ ! -d "vcpkg" ]; then
        git clone --depth=1 https://github.com/microsoft/vcpkg.git vcpkg
    else
        echo "vcpkg directory already exists. Skipping clone."
    fi

    # Bootstrap vcpkg
    if [ -x "vcpkg/bootstrap-vcpkg.sh" ]; then
        vcpkg/bootstrap-vcpkg.sh
    else
        echo "Error: bootstrap-vcpkg.sh not found or not executable."
        exit 1
    fi

    # Install required packages
    vcpkg/vcpkg install glm fmt spdlog argparse opencv4[core,thread,png,fs]

    # Set up CMake integration with vcpkg
    echo "Configuring CMake to use vcpkg toolchain file..."

    # Define the path to the vcpkg toolchain file
    VCPKG_TOOLCHAIN_FILE="$(pwd)/vcpkg/scripts/buildsystems/vcpkg.cmake"

    # Check if the toolchain file exists
    if [ ! -f "$VCPKG_TOOLCHAIN_FILE" ]; then
        echo "Error: vcpkg toolchain file not found at $VCPKG_TOOLCHAIN_FILE."
        exit 1
    fi

    # Create or update CMakeSettings.json for VSCode
    CMAKE_SETTINGS_PATH=".vscode/CMakeSettings.json"

    cat > "$CMAKE_SETTINGS_PATH" <<EOL
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
EOL

    echo "CMakeSettings.json has been configured to use the vcpkg toolchain file."

    # Optionally, inform the user to reload the VSCode window
    echo "If you are using VSCode, consider reloading the window to apply the new CMake settings."

    echo "Setup complete. You can now build the project using CMake."
    echo "1. Run cmake: cmake -B build -DCMAKE_BUILD_TYPE=Release --toolchain $VCPKG_TOOLCHAIN_FILE ."
    echo "2. Run make: cmake --build build --config Release"
    echo "3. Run the executable: ./build/ct_ray_sim --inputPath ./input.png --angles 32"
}

# Function to set up from source
setup_from_source() {
    echo "Setting up from source..."

    # Check if 'build' directory does not exist
    if [ ! -d "build" ]; then
        echo "'build' directory does not exist. Checking connectivity to github.com..."

        # Attempt to reach github.com using curl with a timeout of 5 seconds
        if ! curl -s --head --request GET https://github.com/ --max-time 5 > /dev/null; then
            echo "Error: Unable to reach github.com. Please check your internet connection."
            exit 1
        else
            echo "Successfully connected to github.com."
        fi
    else
        echo "'build' directory already exists. Skipping connectivity check."
    fi

    # Proceed with setup
    cp scripts/resources/fromSource/CMakeLists.txt ./
    cp scripts/resources/fromSource/c_cpp_properties.json .vscode/

    # Optional: Create build directory if it doesn't exist
    if [ ! -d "build" ]; then
        mkdir build
        echo "Created 'build' directory."
    fi

    # Navigate to build directory and run CMake
    cd build || { echo "Failed to enter 'build' directory."; exit 1; }

    # Check if CMakeLists.txt exists in the parent directory
    if [ ! -f "../CMakeLists.txt" ]; then
        echo "Error: CMakeLists.txt not found in the parent directory."
        exit 1
    fi

    echo "Building project using CMake:"
    echo "1. Run cmake: cmake -B build -DCMAKE_BUILD_TYPE=Release ."
    echo "2. Run make: cmake --build build --config Release"
    echo "3. Run the executable: ./build/ct_ray_sim --inputPath ./input.png --angles 32"
}

# Function to display help message
show_help() {
    echo "Usage: ./bootstrap.sh [OPTION]"
    echo "Options:"
    echo "  --nix          Set up using the nix package manager (default)"
    echo "  --vcpkg        Set up using the vcpkg package manager - !!requires git"
    echo "  --fromSource   Set up using cmake from source - !!may be slow"
    echo "  -h, --help     Show this help message"
}

# Function to handle invalid options
invalid_option() {
    echo "Invalid option: $1"
    echo "Use -h or --help to see available options."
    exit 1
}

# Main script logic
main() {
    # Check for required dependencies first
    check_dependencies

    # If no arguments are provided, use the default option
    if [ $# -eq 0 ]; then
        setup_nix
        exit 0
    fi

    # Process the first argument
    case "$1" in
        --nix)
            setup_nix
            ;;
        --vcpkg)
            setup_vcpkg
            ;;
        --fromSource)
            setup_from_source
            ;;
        -h|--help)
            show_help
            ;;
        *)
            invalid_option "$1"
            ;;
    esac
}

# Invoke the main function with all passed arguments
main "$@"
