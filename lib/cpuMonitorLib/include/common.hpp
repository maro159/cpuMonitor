#pragma once
#include <chrono>
#include <vector>

namespace mp {
namespace cpuMonitor {

struct CpuSnapshot {
    std::chrono::system_clock::time_point timestamp{};
    std::vector<double> coresUsage{};
    double totalUsage{};
};

} // namespace cpuMonitor
} // namespace mp
