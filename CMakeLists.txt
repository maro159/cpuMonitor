cmake_minimum_required(VERSION 3.16)
include(CMakePrintHelpers)

project(cpuMonitor
    VERSION 0.2.1
    DESCRIPTION "A multithreaded application for measuring CPU usage on Linux"
    LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
option(UNIT_TESTS "Configure unit tests" ${PROJECT_IS_TOP_LEVEL})

if(UNIT_TESTS)
    message(STATUS "Unit tests ENABLED")
else()
    message(STATUS "Unit tests DISABLED")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
add_compile_options(
    -Wall -Wextra -Wpedantic -Wunused-parameter -Winit-self -Wcast-align
    -Wconversion -Wnull-dereference -Wduplicated-cond -Wsign-conversion -Wlogical-op
)

add_subdirectory(lib/cpuMonitorLib)
add_subdirectory(src)

if(UNIT_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()
