add_executable(cpuMonitor
    main.cpp
)

target_include_directories(cpuMonitor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)