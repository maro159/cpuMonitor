cmake_minimum_required(VERSION 3.16)
include(CMakePrintHelpers)

project(cpuMonitor
    VERSION 0.1
    DESCRIPTION "A multithreaded application for measuring CPU usage on Linux"
    LANGUAGES C CXX
)

option(UNIT_TESTS "Configure unit tests" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
add_compile_options(
    -Wall -Wextra -Wpedantic -Wunused-parameter -Winit-self -Wcast-align
    -Wconversion -Wnull-dereference -Wduplicated-cond -Wsign-conversion -Wlogical-op
)

add_subdirectory(src)

if(UNIT_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()

set(CMAKE_COLOR_DIAGNOSTICS ON)