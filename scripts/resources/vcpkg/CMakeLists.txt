cmake_minimum_required(VERSION 3.10)
project(ct_ray_sim VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Enable CMake integration with vcpkg
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    message(STATUS "Using vcpkg from $ENV{VCPKG_ROOT}")
else()
    set(CMAKE_TOOLCHAIN_FILE "./vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    message(STATUS "Using vcpkg from ./vcpkg")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(ct_ray_sim
    ${CMAKE_SOURCE_DIR}/src/DensityMap.cpp
    ${CMAKE_SOURCE_DIR}/src/Main.cpp
    ${CMAKE_SOURCE_DIR}/src/PostProcessing.cpp
    ${CMAKE_SOURCE_DIR}/src/Ray.cpp
    ${CMAKE_SOURCE_DIR}/src/RayTracer.cpp
    ${CMAKE_SOURCE_DIR}/src/Simulation.cpp
    ${CMAKE_SOURCE_DIR}/src/SimulationResult.cpp
)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ct_ray_sim PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
else()
    target_compile_definitions(ct_ray_sim PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()

# ----------------------------------
# glm
# ----------------------------------
find_package(glm CONFIG REQUIRED)
message(STATUS "Found glm version ${glm_VERSION}")
target_link_libraries(ct_ray_sim glm::glm)

# ----------------------------------
# opencv
# ----------------------------------
find_package(OpenCV REQUIRED)
message(STATUS "Found OpenCV version ${OpenCV_VERSION}")
target_link_libraries(ct_ray_sim ${OpenCV_LIBS})

# ----------------------------------
# spdlog
# ----------------------------------
find_package(spdlog CONFIG REQUIRED)
message(STATUS "Found spdlog version ${spdlog_VERSION}")
target_compile_definitions(ct_ray_sim PRIVATE SPDLOG_FMT_EXTERNAL)
target_link_libraries(ct_ray_sim spdlog::spdlog)

# ----------------------------------
# argparse
# ----------------------------------
find_package(argparse CONFIG REQUIRED)
message(STATUS "Found argparse version ${argparse_VERSION}")
target_link_libraries(ct_ray_sim argparse::argparse)

# ----------------------------------
# fmt
# ----------------------------------
find_package(fmt CONFIG REQUIRED)
message(STATUS "Found fmt version ${fmt_VERSION}")
target_link_libraries(ct_ray_sim fmt::fmt)
