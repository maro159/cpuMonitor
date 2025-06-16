#pragma once
#include "DataWriter.hpp"
#include "SnapshotQueue.hpp"
#include "StatsReader.hpp"
#include <thread>

namespace mp {
namespace cpuMonitor {

/**
 * @brief Main class that manages CPU monitoring and logging threads.
 *
 * The Monitor class launches two threads: one for reading CPU statistics and one for logging/output.
 * It uses a thread-safe queue to pass CPU usage snapshots between threads and supports multiple output writers.
 */
class Monitor {
public:
    /**
     * @brief Construct a new Monitor object.
     * @param refreshInterval Interval between CPU usage reads.
     */
    explicit Monitor(std::chrono::milliseconds refreshInterval = std::chrono::seconds(1));
    /**
     * @brief Start the monitoring and logging threads.
     */
    void start();
    /**
     * @brief Stop the monitoring and logging threads.
     */
    void stop();
    /**
     * @brief Add a new output writer (console, CSV, etc.).
     * @param writer Unique pointer to an IDataWriter implementation.
     */
    void addWriter(std::unique_ptr<IDataWriter> writer);

private:
    void readerThreadFunc(std::stop_token stopToken);
    void loggerThreadFunc(std::stop_token stopToken);
    StatsReader reader_;
    SnapshotQueue queue_;
    std::jthread readerThread_;
    std::jthread loggerThread_;
    std::vector<std::unique_ptr<IDataWriter>> writers_;
    std::chrono::milliseconds refreshInterval_;
};

} // namespace cpuMonitor
} // namespace mp
