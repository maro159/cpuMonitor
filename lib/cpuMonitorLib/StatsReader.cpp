#include "StatsReader.hpp"
#include "common.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace mp {
namespace cpuMonitor {

uint64_t CpuTimes::active() const { return user + nice + system + irq + softirq + steal + guest + guestNice; }
uint64_t CpuTimes::total() const { return active() + idle + iowait; }
CpuTimes CpuTimes::operator-(const CpuTimes &other) const {
    CpuTimes d;
    d.user = user - other.user;
    d.nice = nice - other.nice;
    d.system = system - other.system;
    d.idle = idle - other.idle;
    d.iowait = iowait - other.iowait;
    d.irq = irq - other.irq;
    d.softirq = softirq - other.softirq;
    d.steal = steal - other.steal;
    d.guest = guest - other.guest;
    d.guestNice = guestNice - other.guestNice;
    return d;
}

StatsReader::StatsReader() = default;

CpuTimes StatsReader::parseCpuTimesFromStream(std::istream &iss) {
    CpuTimes times{};
    if (!(iss >> times.user >> times.nice >> times.system >> times.idle)) {
        throw std::runtime_error{"/proc/stat line has fewer than 4 mandatory fields (user, nice, system, idle)"};
    }
    iss >> times.iowait >> times.irq >> times.softirq >> times.steal >> times.guest >> times.guestNice;
    return times;
}

double StatsReader::calcUsagePercent(const CpuTimes &prev, const CpuTimes &curr) {
    uint64_t activeDelta = curr.active() - prev.active();
    uint64_t totalDelta = curr.total() - prev.total();
    if (totalDelta == 0)
        return 0.0;
    return static_cast<double>(activeDelta) / static_cast<double>(totalDelta) * 100.0;
}

std::optional<CpuSnapshot> StatsReader::getSnapshot() {
    std::ifstream stat("/proc/stat");
    if (!stat.is_open()) {
        return std::nullopt; // Could not open /proc/stat
    }
    return getSnapshot(stat);
}

std::optional<CpuSnapshot> StatsReader::getSnapshot(std::istream &stat) {
    const auto timestamp = std::chrono::system_clock::now();
    std::string line;
    std::vector<CpuTimes> coresCpu;
    CpuTimes totalCpu{};
    bool foundCpu = false;

    while (std::getline(stat, line)) {
        if (!line.starts_with("cpu"))
            continue;
        std::istringstream iss(line);
        std::string label;
        iss >> label;
        try {
            CpuTimes times = parseCpuTimesFromStream(iss);
            foundCpu = true;
            if (label == "cpu") {
                totalCpu = std::move(times);
            } else {
                coresCpu.emplace_back(std::move(times));
            }
        } catch (const std::runtime_error &e) {
            std::cerr << "Error parsing CPU times: " << e.what() << std::endl;
            return std::nullopt; // Parsing error, return empty optional
        }
    }

    // If no valid cpu lines were found, return nullopt
    if (!foundCpu) {
        return std::nullopt;
    }

    // no previous result, store current values and return empty
    if (prevCoresCpu_.empty() || !prevTotalCpu_.has_value()) {
        prevTotalCpu_ = std::move(totalCpu);
        prevCoresCpu_ = std::move(coresCpu);
        return std::nullopt;
    }

    CpuSnapshot snapshot{};
    snapshot.timestamp = timestamp;
    snapshot.totalUsage = calcUsagePercent(prevTotalCpu_.value(), totalCpu);
    snapshot.coresUsage.reserve(coresCpu.size());
    for (size_t i = 0; i < coresCpu.size(); ++i) {
        double usage = calcUsagePercent(prevCoresCpu_[i], coresCpu[i]);
        snapshot.coresUsage.push_back(usage);
    }
    prevTotalCpu_ = totalCpu;
    prevCoresCpu_ = std::move(coresCpu);
    return snapshot;
}

} // namespace cpuMonitor
} // namespace mp
