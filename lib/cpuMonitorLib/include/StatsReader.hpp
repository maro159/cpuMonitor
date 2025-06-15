#pragma once
#include "common.hpp"
#include <istream>

namespace mp {
namespace cpuMonitor {

class StatsReader {
public:
    StatsReader() = default;
    CpuSnapshot getSnapshot();
    static CpuSnapshot parseStatStream(std::istream &statStream);
};

} // namespace cpuMonitor
} // namespace mp
