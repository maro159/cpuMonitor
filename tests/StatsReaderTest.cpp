#include "StatsReader.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

using namespace mp::cpuMonitor;

class StatsReaderFixture : public ::testing::Test {
protected:
    StatsReader reader;
    static inline const std::string stat1 = "cpu  100 200 300 400 500 600 700 800\n"
                                            "cpu0 10 20 30 40 50 60 70 80\n"
                                            "cpu1 90 180 270 360 450 540 630 720\n";
    static inline const std::string stat2 = "cpu  200 400 600 800 1000 1200 1400 1600\n"
                                            "cpu0 20 40 60 80 100 120 140 160\n"
                                            "cpu1 180 360 540 720 900 1080 1260 1440\n";
    std::stringstream stat1_stream;
    std::stringstream stat2_stream;
    std::stringstream empty_stream;
    std::stringstream invalid_stream;
    std::stringstream missing_fields_stream;

    void SetUp() override {
        stat1_stream.str(stat1);
        stat2_stream.str(stat2);
        empty_stream.str("");
        invalid_stream.str("invalid data");
        missing_fields_stream.str("cpu 100 200 300\n");
    }
};

TEST_F(StatsReaderFixture, DeltaUsageTypicalInput_CorrectRange) {
    std::optional<CpuSnapshot> snapshotOpt;
    snapshotOpt = reader.getSnapshot(stat1_stream);
    ASSERT_FALSE(snapshotOpt.has_value());
    snapshotOpt = reader.getSnapshot(stat2_stream);
    ASSERT_TRUE(snapshotOpt.has_value());
    const auto &snapshot = *snapshotOpt;
    ASSERT_EQ(snapshot.coresUsage.size(), 2);
    for (auto usage : snapshot.coresUsage) {
        EXPECT_GE(usage, 0.0);
        EXPECT_LE(usage, 100.0);
    }
    EXPECT_GE(snapshot.totalUsage, 0.0);
    EXPECT_LE(snapshot.totalUsage, 100.0);
}

TEST_F(StatsReaderFixture, HandlesEmptyInput) {
    reader.getSnapshot(stat1_stream);
    auto snapshotOpt = reader.getSnapshot(empty_stream);
    EXPECT_FALSE(snapshotOpt.has_value());
}

TEST_F(StatsReaderFixture, HandlesInvalidInput) {
    reader.getSnapshot(stat1_stream);
    auto snapshotOpt = reader.getSnapshot(invalid_stream);
    EXPECT_FALSE(snapshotOpt.has_value());
}

TEST_F(StatsReaderFixture, HandlesMissingFields) {
    reader.getSnapshot(stat1_stream);
    auto snapshotOpt = reader.getSnapshot(missing_fields_stream);
    EXPECT_FALSE(snapshotOpt.has_value());
}

TEST_F(StatsReaderFixture, CalcUsagePercent) {
    // Simulate two /proc/stat snapshots for delta
    std::ignore = reader.getSnapshot(stat1_stream);
    auto snapshotOpt = reader.getSnapshot(stat2_stream);
    ASSERT_TRUE(snapshotOpt.has_value());
    const auto &snapshot = *snapshotOpt;
    // Calculate expected value for totalUsage
    double expected_total = 75.0;
    EXPECT_DOUBLE_EQ(snapshot.totalUsage, expected_total);
    // Calculate expected values for cores
    double expected_core0 = 75.0;
    double expected_core1 = 75.0;
    ASSERT_EQ(snapshot.coresUsage.size(), 2);
    EXPECT_DOUBLE_EQ(snapshot.coresUsage[0], expected_core0);
    EXPECT_DOUBLE_EQ(snapshot.coresUsage[1], expected_core1);
}

TEST_F(StatsReaderFixture, HandlesMinFieldsCount) {
    // 4 fields: user, nice, system, idle (minimum valid)
    std::stringstream min_fields_stream("cpu 1 2 3 4\n");
    reader.getSnapshot(stat1_stream); // prime previous
    auto snapshotOpt = reader.getSnapshot(min_fields_stream);
    EXPECT_TRUE(snapshotOpt.has_value());
}

TEST_F(StatsReaderFixture, HandlesMaxFieldsCount) {
    // 10 fields: user, nice, system, idle, iowait, irq, softirq, steal, guest, guestNice (maximum typical)
    std::stringstream max_fields_stream("cpu 1 2 3 4 5 6 7 8 9 10\n");
    reader.getSnapshot(stat1_stream); // prime previous
    auto snapshotOpt = reader.getSnapshot(max_fields_stream);
    EXPECT_TRUE(snapshotOpt.has_value());
}
