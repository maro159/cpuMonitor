#pragma once
#include <chrono>
#include <vector>

namespace mp {
namespace cpuMonitor {

using CpuCoreUsage = double;

struct CpuSnapshot {
    std::chrono::system_clock::time_point timestamp{};
    std::vector<CpuCoreUsage> coresUsage{};
    CpuCoreUsage totalUsage{};
};

} // namespace cpuMonitor
} // namespace mp
