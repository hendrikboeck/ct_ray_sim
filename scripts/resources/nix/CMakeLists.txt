cmake_minimum_required(VERSION 3.10)
project(ct_ray_sim VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

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
find_package(glm)

if(glm_FOUND)
    message(STATUS "Found glm version ${glm_VERSION}")
    include_directories(${glm_INCLUDE_DIRS})
    target_link_libraries(ct_ray_sim glm::glm)
else()
    message(FATAL_ERROR "glm not found. Please install glm.")
endif()

# ----------------------------------
# opencv
# ----------------------------------
find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
    message(STATUS "Found OpenCV version ${OpenCV_VERSION}")
    include_directories(${OpenCV_INCLUDE_DIRS})
    target_link_libraries(ct_ray_sim ${OpenCV_LIBS})
else()
    message(FATAL_ERROR "OpenCV not found. Please install OpenCV.")
endif()

# ----------------------------------
# spdlog
# ----------------------------------
find_package(spdlog REQUIRED)

if(spdlog_FOUND)
    message(STATUS "Found spdlog version ${spdlog_VERSION}")
    include_directories(${spdlog_INCLUDE_DIRS})
    target_compile_definitions(ct_ray_sim PRIVATE SPDLOG_FMT_EXTERNAL)
    target_link_libraries(ct_ray_sim spdlog::spdlog)
else()
    message(FATAL_ERROR "spdlog not found. Please install spdlog.")
endif()

# ----------------------------------
# argparse
# ----------------------------------
find_package(argparse REQUIRED)

if(argparse_FOUND)
    message(STATUS "Found argparse version ${argparse_VERSION}")
    include_directories(${argparse_INCLUDE_DIRS})
    target_link_libraries(ct_ray_sim argparse::argparse)
else()
    message(FATAL_ERROR "argparse not found. Please install argparse.")
endif()

# ----------------------------------
# fmt
# ----------------------------------
find_package(fmt REQUIRED)

if(fmt_FOUND)
    message(STATUS "Found fmt version ${fmt_VERSION}")
    include_directories(${fmt_INCLUDE_DIRS})
    target_link_libraries(ct_ray_sim fmt::fmt)
else()
    message(FATAL_ERROR "fmt not found. Please install fmt.")
endif()