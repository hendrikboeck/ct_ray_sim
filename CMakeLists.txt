cmake_minimum_required(VERSION 3.10)
project(ct_ray_sim VERSION 0.1.0)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/build/_deps/glm-src
    ${CMAKE_SOURCE_DIR}/build/_deps/opencv-src/include
    ${CMAKE_SOURCE_DIR}/build/_deps/spdlog-src/include
    ${CMAKE_SOURCE_DIR}/build/_deps/argparse-src/include
    ${CMAKE_SOURCE_DIR}/build/_deps/fmt-src/include
)
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
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)
target_link_libraries(ct_ray_sim glm::glm)

# ----------------------------------
# opencv
# ----------------------------------
FetchContent_Declare(
    opencv
    GIT_REPOSITORY https://github.com/opencv/opencv.git
    GIT_TAG 4.10.0
)
FetchContent_MakeAvailable(opencv)
target_link_libraries(ct_ray_sim opencv_core opencv_imgproc opencv_highgui)

# ----------------------------------
# spdlog
# ----------------------------------
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(ct_ray_sim spdlog::spdlog)

# ----------------------------------
# argparse
# ----------------------------------
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.1
)
FetchContent_MakeAvailable(argparse)
target_link_libraries(ct_ray_sim argparse::argparse)

# ----------------------------------
# fmt
# ----------------------------------
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.0.2
)
FetchContent_MakeAvailable(fmt)
target_link_libraries(ct_ray_sim fmt::fmt)
