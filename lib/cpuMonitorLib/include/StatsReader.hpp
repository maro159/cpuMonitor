#pragma once
#include "common.hpp"
#include <istream>
#include <optional>
#include <vector>
#include <fstream>

namespace mp {
namespace cpuMonitor {

struct CpuTimes {
    uint64_t user{0}, nice{0}, system{0}, idle{0}, iowait{0}, irq{0}, softirq{0}, steal{0}, guest{0}, guestNice{0};
    uint64_t active() const;
    uint64_t total() const;
    CpuTimes operator-(const CpuTimes &other) const;
};

class StatsReader {
public:
    StatsReader();
    std::optional<CpuSnapshot> getSnapshot()
    {
        std::ifstream stat("/proc/stat");
        if (!stat.is_open()) {
            return std::nullopt; // Could not open /proc/stat
        }
        return getSnapshot(stat);
    }
    std::optional<CpuSnapshot> getSnapshot(std::istream &stat);

private:
    std::optional<CpuTimes> prevTotalCpu_{};
    std::vector<CpuTimes> prevCoresCpu_{};
    static CpuTimes parseCpuTimesFromStream(std::istream &iss);
    static double calcUsagePercent(const CpuTimes &prev, const CpuTimes &curr);
};

} // namespace cpuMonitor
} // namespace mp
