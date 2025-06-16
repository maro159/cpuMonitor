#pragma once

#include "common.hpp"
#include <fstream>
#include <string_view>

namespace mp {
namespace cpuMonitor {

/**
 * @brief Interface for writing CPU usage data.
 */
class IDataWriter {
public:
    virtual ~IDataWriter() = default;
    /**
     * @brief Write a CPU usage snapshot.
     * @param snapshot The CPU usage data to write.
     */
    virtual void write(const CpuSnapshot &snapshot) = 0;
};

/**
 * @brief Writes CPU usage data to the console.
 */
class ConsoleDataWriter : public IDataWriter {
public:
    ConsoleDataWriter();
    void write(const CpuSnapshot &snapshot) override;
};

/**
 * @brief Writes CPU usage data to a CSV file.
 */
class CsvDataWriter : public IDataWriter {
public:
    explicit CsvDataWriter(std::string_view filename);
    void write(const CpuSnapshot &snapshot) override;

private:
    std::ofstream file_;
    bool headerWritten_ = false;
};

} // namespace cpuMonitor
} // namespace mp
