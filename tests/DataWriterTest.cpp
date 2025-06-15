#include "DataWriter.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

using namespace mp::cpuMonitor;

namespace {
CpuSnapshot makeTestSnapshot() {
    CpuSnapshot snap;
    snap.timestamp = std::chrono::system_clock::now();
    snap.totalUsage = 42.0;
    snap.coresUsage = {1.1, 2.2, 3.3};
    return snap;
}
} // namespace

TEST(DataWriterTest, ConsoleDataWriterWrites) {
    // Redirect std::cout
    std::stringstream buffer;
    std::streambuf *oldCout = std::cout.rdbuf(buffer.rdbuf());
    ConsoleDataWriter writer;
    CpuSnapshot snap = makeTestSnapshot();
    writer.write(snap);
    std::cout.rdbuf(oldCout);
    std::string output = buffer.str();
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("1.1"), std::string::npos);
    EXPECT_NE(output.find("2.2"), std::string::npos);
    EXPECT_NE(output.find("3.3"), std::string::npos);
}

TEST(DataWriterTest, CsvDataWriterWritesFile) {
    auto temp = ::testing::TempDir();
    std::string filename = std::filesystem::path(temp) / "test_output.csv";
    {
        CsvDataWriter writer(std::string_view{filename});
        CpuSnapshot snap = makeTestSnapshot();
        writer.write(snap);
    }
    std::ifstream file(filename);
    ASSERT_TRUE(file.is_open());
    std::string content, line;
    while (std::getline(file, line)) {
        content += line + '\n';
    }
    file.close();
    EXPECT_NE(content.find("1.1"), std::string::npos);
    EXPECT_NE(content.find("2.2"), std::string::npos);
    EXPECT_NE(content.find("3.3"), std::string::npos);
}
