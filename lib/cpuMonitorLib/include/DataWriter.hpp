#pragma once

#include "common.hpp"

#include <fstream>
#include <string_view>

namespace mp {
namespace cpuMonitor {

class IDataWriter {
public:
    virtual ~IDataWriter() = default;
    virtual void write(CpuSnapshot const &snapshot) = 0;
};

class ConsoleDataWriter : public IDataWriter {
public:
    ConsoleDataWriter();
    void write(CpuSnapshot const &snapshot) override;
};

class CsvDataWriter : public IDataWriter {
public:
    CsvDataWriter(std::string_view filename);
    void write(CpuSnapshot const &snapshot) override;

private:
    std::ofstream file_;
    bool headerWritten_ = false;
};

} // namespace cpuMonitor
} // namespace mp
