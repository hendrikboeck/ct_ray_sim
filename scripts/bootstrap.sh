#!/usr/bin/env bash

# Default action if no options are provided
DEFAULT_OPTION="--nix"

# Check for provided options and call respective script
case "$1" in
    --nix)
        echo "Setting up using nix..."
        cp scripts/resources/nix/.envrc ./
        cp scripts/resources/nix/shell.nix ./
        cp scripts/resources/nix/CMakeLists.txt ./
        direnv allow .
        ;;
    --vcpkg)
        echo "Setting up using vcpkg..."
        cp scripts/resources/vcpkg/CMakeLists.txt ./
        git clone https://github.com/microsoft/vcpkg.git vcpkg
        cd vcpkg
        ./bootstrap-vcpkg.sh
        cd ..
        vcpkg/vcpkg install opencv4 glm spdlog fmt argparse
        ;;
    --fromSource)
        echo "Setting up from source..."
        cp scripts/resources/fromSource/CMakeLists.txt ./
        cp scripts/resources/fromSource/c_cpp_properties.json .vscode/
        ;;
    -h|--help)
        echo "Usage: ./bootstrap.sh [OPTION]"
        echo "Options:"
        echo "  --nix          Set up using the nix package manager (default)"
        echo "  --vcpkg        Set up using the vcpkg package manager - !!requires git"
        echo "  --fromSource   Set up using cmake from source - !!may be slow"
        echo "  -h, --help     Show this help message"
        ;;
    *)
        echo "Invalid option: $1"
        echo "Use -h or --help to see available options."
        exit 1
        ;;
esac
