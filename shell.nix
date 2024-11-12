{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  name = "cpp20-dev-env";

  buildInputs = with pkgs; [
    gcc
    gdb
    cmake
    git
    vcpkg
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
    export CMAKE_BUILD_TYPE=Release
    export CMAKE_CXX_STANDARD=20
  '';
}
