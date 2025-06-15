#include "StatsReader.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace mp::cpuMonitor;

TEST(StatsReaderTest, ParsesTypicalInput_CorrectRange) {
    std::string testProcStat = "cpu  100 200 300 400 500 600 700 800\n"
                               "cpu0 10 20 30 40 50 60 70 80\n"
                               "cpu1 90 180 270 360 450 540 630 720\n";
    std::istringstream iss(testProcStat);
    CpuSnapshot snapshot = StatsReader::parseStatStream(iss);

    ASSERT_EQ(snapshot.coresUsage.size(), 2);
    // Check that usage is calculated and in expected range
    for (auto usage : snapshot.coresUsage) {
        EXPECT_GE(usage, 0.0);
        EXPECT_LE(usage, 100.0);
    }
    EXPECT_GE(snapshot.totalUsage, 0.0);
    EXPECT_LE(snapshot.totalUsage, 100.0);
}

TEST(StatsReaderTest, HandlesEmptyInput) {
    std::istringstream iss("");
    CpuSnapshot snapshot = StatsReader::parseStatStream(iss);
    EXPECT_TRUE(snapshot.coresUsage.empty());
    EXPECT_EQ(snapshot.totalUsage, 0.0);
}

TEST(StatsReaderTest, HandlesInvalidInput) {
    std::string invalidInput = "invalid data";
    std::istringstream iss(invalidInput);
    CpuSnapshot snapshot = StatsReader::parseStatStream(iss);
    EXPECT_TRUE(snapshot.coresUsage.empty());
    EXPECT_EQ(snapshot.totalUsage, 0.0);
}

TEST(StatsReaderTest, HandlesMissingFields) {
    std::string missingFields = "cpu 100 200 300\n"; // Missing idle and other fields
    std::istringstream iss(missingFields);
    auto snapshot = StatsReader::parseStatStream(iss);
    EXPECT_EQ(snapshot.totalUsage, 0.0); // No idle time means no usage can be calculated
    EXPECT_TRUE(snapshot.coresUsage.empty());
}

TEST(StatsReaderTest, ParsesMinimumFields_CorrectUsage) {
    // NOTE: older Linux kernels may have fewer fields in /proc/stat
    // fields are: user, nice, system, idle
    std::string singleCoreStat = "cpu 200 200 200 400\n";
    std::istringstream iss(singleCoreStat);
    CpuSnapshot snapshot = StatsReader::parseStatStream(iss);

    // should be 60% usage - idle is 400 and total is 1000
    EXPECT_DOUBLE_EQ(snapshot.totalUsage, 60.0);
}

TEST(StatsReaderTest, ParsesMaximumFields_CorrectUsage) {
    // NOTE: according to man page, /proc/stat can have up to 10 fields per core
    // fields are: user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice
    // iowait should be included in idle
    std::string singleCoreStat = "cpu 200 200 200 200 400 400 200 200 200 200\n";
    std::istringstream iss(singleCoreStat);
    CpuSnapshot snapshot = StatsReader::parseStatStream(iss);

    // should be 75% usage - idle + iowait = 800 and total=2400
    EXPECT_DOUBLE_EQ(snapshot.totalUsage, 75.0);
}

TEST(StatsReaderTest, ParsesMultiCore_CorrectUsage) {
    // Two cores, typical /proc/stat format
    std::string multiCoreStat = "cpu  100 200 300 400 500 600 700 800\n"
                                "cpu0 10 20 30 40 50 60 70 80\n"
                                "cpu1 90 180 270 360 450 540 630 720\n";
    std::istringstream iss(multiCoreStat);
    CpuSnapshot snapshot = StatsReader::parseStatStream(iss);

    ASSERT_EQ(snapshot.coresUsage.size(), 2);
    // For cpu0: active = 10+20+30+60+70+80=270, idle=40+50=90, total=360
    // usage = 270/360*100 = 75.0
    // For cpu1: active = 90+180+270+540+630+720=2430, idle=360+450=810, total=3240
    // usage = 2430/3240*100 = 75.0
    EXPECT_DOUBLE_EQ(snapshot.coresUsage[0], 75.0);
    EXPECT_DOUBLE_EQ(snapshot.coresUsage[1], 75.0);
}
