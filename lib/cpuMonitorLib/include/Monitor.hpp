#pragma once
#include "DataWriter.hpp"
#include "SnapshotQueue.hpp"
#include "StatsReader.hpp"
#include <thread>

namespace mp {
namespace cpuMonitor {

class Monitor {
public:
    explicit Monitor(std::chrono::milliseconds refreshInterval = std::chrono::seconds(1));
    void start();
    void stop();
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
