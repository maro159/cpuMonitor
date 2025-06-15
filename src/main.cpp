#include "helpers.hpp"
#include <DataWriter.hpp>
#include <Monitor.hpp>
#include <atomic>
#include <csignal>
#include <iostream>
#include <thread>

using namespace mp::cpuMonitor;
using namespace std::chrono_literals;
using namespace mp::cpuMonitor::helpers;

static constexpr int kDefaultRefreshMs = 1000;

namespace {
std::atomic<bool> running{true};
void signalHandler(int) { running = false; }
} // namespace

int main(int argc, char *argv[]) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    if (handleMetaArgs(argc, argv))
        return 0;

    int refreshMs = parseInterval(argc, argv, kDefaultRefreshMs);
    auto refreshInterval = std::chrono::milliseconds(refreshMs);
    std::cout << "CPU Monitor started. Press Ctrl+C to exit.\n";
    std::cout << "Refresh interval set to " << refreshInterval.count() << " ms.\n";

    Monitor monitor(refreshInterval);
    monitor.addWriter(std::make_unique<ConsoleDataWriter>());
    monitor.addWriter(std::make_unique<CsvDataWriter>("cpu_usage.csv"));
    monitor.start();

    while (running) {
        std::this_thread::sleep_for(500ms);
    }

    monitor.stop();
    std::cout << "\nCPU Monitor stopped.\n";
    return 0;
}
