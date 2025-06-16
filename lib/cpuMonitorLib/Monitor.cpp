#include "Monitor.hpp"
#include "DataWriter.hpp"
#include "SnapshotQueue.hpp"
#include "StatsReader.hpp"
#include <chrono>
#include <thread>

namespace mp {
namespace cpuMonitor {

using namespace std::chrono_literals;
static constexpr size_t kQueueCapacity = 1000;

Monitor::Monitor(std::chrono::milliseconds refreshInterval)
    : queue_(kQueueCapacity), refreshInterval_(refreshInterval) {}

void Monitor::start() {
    readerThread_ = std::jthread([this](std::stop_token st) { readerThreadFunc(st); });
    loggerThread_ = std::jthread([this](std::stop_token st) { loggerThreadFunc(st); });
}

void Monitor::stop() {
    readerThread_.request_stop();
    loggerThread_.request_stop();
    // Wait for threads to finish
    if (readerThread_.joinable()) {
        readerThread_.join();
    }
    if (loggerThread_.joinable()) {
        loggerThread_.join();
    }
    // jthreads are automatically joined on destruction, but we join explicitly here for prompt shutdown
}

void Monitor::addWriter(std::unique_ptr<IDataWriter> writer) {
    if (!writer)
        return;
    writers_.emplace_back(std::move(writer));
}

void Monitor::readerThreadFunc(std::stop_token st) {
    while (!st.stop_requested()) {
        auto snapshot = reader_.getSnapshot();
        if (snapshot.has_value())
            queue_.pushSnapshot(snapshot.value()); // Pass the value if present
        std::this_thread::sleep_for(refreshInterval_);
    }
}

void Monitor::loggerThreadFunc(std::stop_token st) {
    while (!st.stop_requested()) {
        // Wait for a snapshot to be available or timeout
        auto optSnapshot = queue_.popSnapshot(refreshInterval_);
        if (optSnapshot.has_value()) {
            for (const auto &writer : writers_) {
                writer->write(*optSnapshot);
            }
        }
    }
}

} // namespace cpuMonitor
} // namespace mp
