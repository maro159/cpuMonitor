#include "StatsReader.hpp"
#include "common.hpp"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace mp {
namespace cpuMonitor {

namespace {
struct CpuTimes {
    uint64_t user{0}, nice{0}, system{0}, idle{0}, iowait{0}, irq{0}, softirq{0}, steal{0}, guest{0}, guestNice{0};
    uint64_t active() const { return user + nice + system + irq + softirq + steal + guest + guestNice; }
    uint64_t total() const { return active() + idle + iowait; }
};
} // namespace

static double calcPercentUsage(const CpuTimes &cpu) {
    if (cpu.total() == 0) {
        return 0.0;
    }
    return static_cast<double>(cpu.active()) / static_cast<double>(cpu.total()) * 100.0;
}

CpuSnapshot StatsReader::getSnapshot() {
    std::ifstream stat("/proc/stat");
    return parseStatStream(stat);
}

CpuSnapshot StatsReader::parseStatStream(std::istream &statStream) {
    CpuSnapshot snapshot{};
    snapshot.timestamp = std::chrono::system_clock::now();

    std::string line;
    std::vector<CpuTimes> coresCpu;
    CpuTimes totalCpu{};

    while (std::getline(statStream, line)) {
        if (line.starts_with("cpu")) {
            std::istringstream iss(line);
            std::string label;
            CpuTimes times{};
            iss >> label >> times.user >> times.nice >> times.system >> times.idle >> times.iowait >> times.irq >>
                times.softirq >> times.steal;
            if (label == "cpu") {
                totalCpu = std::move(times);
            } else {
                coresCpu.emplace_back(std::move(times));
            }
        } else {
            break;
        }
    }

    snapshot.coresUsage.reserve(coresCpu.size());
    for (const auto &core : coresCpu) {
        snapshot.coresUsage.emplace_back(calcPercentUsage(core));
    }
    snapshot.totalUsage = calcPercentUsage(totalCpu);

    return snapshot;
}

} // namespace cpuMonitor
} // namespace mp
