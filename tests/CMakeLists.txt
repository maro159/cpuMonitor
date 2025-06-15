include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
    DummyTest.cpp
    StatsReaderTest.cpp
    SnapshotQueueTest.cpp
    DataWriterTest.cpp
)

target_link_libraries(unit_tests PRIVATE
    GTest::gtest_main
    cpuMonitorLib
)

include(GoogleTest)
gtest_discover_tests(unit_tests)