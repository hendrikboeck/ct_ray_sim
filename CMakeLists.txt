cmake_minimum_required(VERSION 3.10)
project(tomographix VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(tomographix
    ${CMAKE_SOURCE_DIR}/src/ct_sim.cpp
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)
include_directories(${CMAKE_SOURCE_DIR}/include)

# ----------------------------------
# glm
# ----------------------------------
find_package(glm)

if(glm_FOUND)
    message(STATUS "Found glm version ${glm_VERSION}")
    include_directories(${glm_INCLUDE_DIRS})
    target_link_libraries(tomographix glm::glm)
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
    target_link_libraries(tomographix ${OpenCV_LIBS})
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
    target_link_libraries(tomographix spdlog::spdlog)
else()
    message(FATAL_ERROR "spdlog not found. Please install spdlog.")
endif()