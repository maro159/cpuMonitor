#pragma once

#include <chrono>
#include <vector>

namespace mp {
namespace cpuMonitor {

/**
 * @brief Represents a snapshot of CPU usage at a specific time.
 */
struct CpuSnapshot {
    std::chrono::system_clock::time_point timestamp{}; // Time of the snapshot
    std::vector<double> coresUsage{};                  // Per-core CPU usage percentages
    double totalUsage{};                               // Total CPU usage percentage
};

} // namespace cpuMonitor
} // namespace mp
