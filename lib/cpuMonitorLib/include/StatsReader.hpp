#pragma once

#include "common.hpp"

#include <fstream>
#include <istream>
#include <optional>
#include <vector>

namespace mp {
namespace cpuMonitor {

/**
 * @brief Stores CPU time statistics for a single core or total CPU.
 */
struct CpuTimes {
    uint64_t user{0}, nice{0}, system{0}, idle{0}, iowait{0}, irq{0}, softirq{0}, steal{0}, guest{0}, guestNice{0};
    uint64_t active() const;
    uint64_t total() const;
    CpuTimes operator-(const CpuTimes &other) const;
};

/**
 * @brief Reads and computes CPU usage statistics from /proc/stat.
 */
class StatsReader {
public:
    /**
     * @brief Construct a new StatsReader object.
     */
    StatsReader();
    /**
     * @brief Get a CPU usage snapshot from /proc/stat.
     * @return Optional CpuSnapshot if successful, std::nullopt otherwise.
     */
    std::optional<CpuSnapshot> getSnapshot();
    /**
     * @brief Get a CPU usage snapshot from a provided stream.
     * @param stat Input stream (e.g., file or stringstream)
     * @return Optional CpuSnapshot if successful, std::nullopt otherwise.
     */
    std::optional<CpuSnapshot> getSnapshot(std::istream &stat);

private:
    std::optional<CpuTimes> prevTotalCpu_{};
    std::vector<CpuTimes> prevCoresCpu_{};
    static CpuTimes parseCpuTimesFromStream(std::istream &iss);
    static double calcUsagePercent(const CpuTimes &prev, const CpuTimes &curr);
};

} // namespace cpuMonitor
} // namespace mp
