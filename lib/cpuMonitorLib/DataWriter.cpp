#include "DataWriter.hpp"
#include <iostream>
#include <sstream>

namespace mp {
namespace cpuMonitor {

ConsoleDataWriter::ConsoleDataWriter() { std::cout << "\n"; }

CsvDataWriter::CsvDataWriter(std::string_view filename) { file_.open(filename.data(), std::ios::out | std::ios::app); }

void ConsoleDataWriter::write(CpuSnapshot const &snapshot) {
    // Move cursor to the beginning of the line
    std::cout << "\r";
    std::ostringstream oss;
    for (size_t i = 0; i < snapshot.coresUsage.size(); ++i) {
        oss << "CPU" << i << ": " << std::fixed << std::setprecision(2) << snapshot.coresUsage[i] << "% ";
    }
    oss << "TOTAL: " << std::fixed << std::setprecision(2) << snapshot.totalUsage << "%";
    std::cout << oss.str() << std::flush;
}

void CsvDataWriter::write(CpuSnapshot const &snapshot) {
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open file for writing");
    }

    if (!headerWritten_) {
        auto numCores = snapshot.coresUsage.size();
        file_ << "timestamp,";
        for (size_t i = 0; i < numCores; ++i) {
            file_ << "cpu" << i << ",";
        }
        file_ << "total\n";
        headerWritten_ = true;
    }

    auto timestamp = std::chrono::system_clock::to_time_t(snapshot.timestamp);
    file_ << timestamp << ",";
    for (const auto &usage : snapshot.coresUsage) {
        file_ << std::fixed << usage << ",";
    }
    file_ << std::fixed << snapshot.totalUsage << "\n";
}

} // namespace cpuMonitor
} // namespace mp
