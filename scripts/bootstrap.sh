#!/bin/bash

# Default action if no options are provided
DEFAULT_OPTION="--nix"

# Check for provided options and call respective script
case "$1" in
    --nix)
        echo "Setting up Nix environment..."
        cp scripts/resources/nix/* ./
        direnv allow .
        ;;
    --vcpkg)
        echo "Setting up vcpkg..."
        # Add vcpkg setup commands here
        ;;
    --git)
        echo "Setting up Git..."
        # Add git setup commands here
        ;;
    -h|--help)
        echo "Usage: bootstrap [OPTION]"
        echo "Options:"
        echo "  --nix      Set up the Nix environment"
        echo "  --vcpkg    Set up vcpkg package manager"
        echo "  --git      Set up Git configuration"
        echo "  -h, --help Show this help message"
        ;;
    *)
        echo "Invalid option: $1"
        echo "Use -h or --help to see available options."
        exit 1
        ;;
esac
