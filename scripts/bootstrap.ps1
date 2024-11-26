# Default action if no options are provided
$DefaultOption = "--vcpkg"

# Check for provided options and call respective script
param (
    [string]$Option = $DefaultOption
)

switch ($Option) {
    "--vcpkg" {
        Write-Output "Setting up using vcpkg..."
        Copy-Item -Path "scripts/resources/vcpkg/CMakeLists.txt" -Destination "./"
        git clone https://github.com/microsoft/vcpkg.git vcpkg
        cd vcpkg
        .\bootstrap-vcpkg.bat
        cd ..
        .\vcpkg\vcpkg.exe install opencv4 glm spdlog fmt argparse
    }
    "--fromSource" {
        Write-Output "Setting up from source..."
        Copy-Item -Path "scripts/resources/fromSource/CMakeLists.txt" -Destination "./"
        Copy-Item -Path "scripts/resources/fromSource/c_cpp_properties.json" -Destination ".vscode/" -Force
    }
    "-h" | "--help" {
        Write-Output "Usage: .\bootstrap.ps1 [OPTION]"
        Write-Output "Options:"
        Write-Output "  --vcpkg        Set up using the vcpkg package manager - !!requires git"
        Write-Output "  --fromSource   Set up using cmake from source - !!may be slow"
        Write-Output "  -h, --help     Show this help message"
    }
    Default {
        Write-Output "Invalid option: $Option"
        Write-Output "Use -h or --help to see available options."
        exit 1
    }
}
