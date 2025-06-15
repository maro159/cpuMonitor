#include "Monitor.hpp"
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace mp::cpuMonitor;

namespace {
struct DummyDataWriter : IDataWriter {
    std::vector<CpuSnapshot> written;
    void write(const CpuSnapshot &snap) override { written.push_back(snap); }
};
} // namespace

TEST(MonitorTest, StartsAndStops) {
    Monitor monitor;
    EXPECT_NO_THROW(monitor.start());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_NO_THROW(monitor.stop());
}

TEST(MonitorTest, WritesSnapshots) {
    DummyDataWriter writer;
    Monitor monitor;
    monitor.setDataWriter(&writer);
    monitor.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    monitor.stop();
    EXPECT_FALSE(writer.written.empty());
}

TEST(MonitorTest, ThreadsStopOnDestruction) {
    DummyDataWriter writer;
    {
        Monitor monitor;
        monitor.setDataWriter(&writer);
        monitor.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Monitor goes out of scope and should stop threads automatically
    }
    SUCCEED(); // If we reach here, no deadlock or crash occurred
}
