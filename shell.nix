{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  name = "cpp20-dev-env";

  buildInputs = with pkgs; [
    gcc # Add GCC compiler
    gdb # GNU Debugger (optional but recommended)
    spdlog
    glm
    cmake
    opencv
    fmt
    argparse
    pkg-config
    git
  ];

  shellHook = ''
    # Set GCC as the default compiler
    export CC=${pkgs.gcc}/bin/gcc
    export CXX=${pkgs.gcc}/bin/g++

    # Compiler flags for C++20 and additional warnings
    export CXXFLAGS="-std=c++20 -Wall -Wextra -Wpedantic"

    # CMake environment variables
    export CMAKE_C_COMPILER=$CC
    export CMAKE_CXX_COMPILER=$CXX
    export CMAKE_BUILD_TYPE=Debug                 # Set default build type to Debug
    export CMAKE_CXX_STANDARD=20                  # Set C++20 standard

    # Extract include paths for OpenCV and glm
    opencv_include=$(pkg-config --cflags-only-I opencv4 | sed 's/-I//g' | tr ' ' '\n')

    # Update c_cpp_properties.json
    mkdir -p .vscode
    cat > .vscode/c_cpp_properties.json <<- EOM
    {
      "configurations": [
        {
          "name": "Nix",
          "includePath": [
            "include",
            "$(pkg-config --cflags-only-I opencv4 | sed 's/-I//g' | tr ' ' '\n')",
            "${pkgs.glm}/include",
            "${pkgs.spdlog.dev}/include",
            "${pkgs.argparse}/include",
            "${pkgs.fmt.dev}/include"
          ],
          "defines": [
            "SPDLOG_FMT_EXTERNAL"
          ],
          "compilerPath": "$CXX",
          "cStandard": "c11",
          "cppStandard": "c++20",
          "intelliSenseMode": "gcc-x64"
        }
      ],
      "version": 4
    }
    EOM

    echo "c_cpp_properties.json updated with OpenCV and glm include paths."
  '';
}
