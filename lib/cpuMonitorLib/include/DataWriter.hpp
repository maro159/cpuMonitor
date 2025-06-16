#pragma once

#include "common.hpp"
#include <fstream>
#include <string_view>

namespace mp {
namespace cpuMonitor {

class IDataWriter {
public:
    virtual ~IDataWriter() = default;
    virtual void write(const CpuSnapshot &snapshot) = 0;
};

class ConsoleDataWriter : public IDataWriter {
public:
    ConsoleDataWriter();
    void write(const CpuSnapshot &snapshot) override;
};

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
