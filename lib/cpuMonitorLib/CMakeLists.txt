add_library(cpuMonitorLib STATIC
    DataWriter.cpp
    Monitor.cpp
    SnapshotQueue.cpp
    StatsReader.cpp
)

target_include_directories(cpuMonitorLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)