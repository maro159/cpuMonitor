add_executable(cpuMonitor
    main.cpp
)

target_include_directories(cpuMonitor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}  # For version.hpp
)

target_link_libraries(cpuMonitor PRIVATE
    cpuMonitorLib
)

configure_file(include/version.hpp.in version.hpp)