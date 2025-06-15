#include "SnapshotQueue.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace mp::cpuMonitor;

namespace {

CpuSnapshot makeSnapshot(int id) {
    CpuSnapshot snap;
    snap.timestamp = std::chrono::system_clock::now();
    snap.totalUsage = static_cast<double>(id * 10);
    snap.coresUsage = std::vector<double>(1, static_cast<double>(id));
    return snap;
}

} // namespace

TEST(SnapshotQueueTest, EmptySizeCapacity) {
    SnapshotQueue queue(3);
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);
    EXPECT_EQ(queue.capacity(), 3u);
    queue.pushSnapshot(makeSnapshot(1));
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1u);
    EXPECT_EQ(queue.capacity(), 3u);
    queue.pushSnapshot(makeSnapshot(2));
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 2u);
    EXPECT_EQ(queue.capacity(), 3u);
    queue.popSnapshot();
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1u);
    EXPECT_EQ(queue.capacity(), 3u);
    queue.popSnapshot();
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);
    EXPECT_EQ(queue.capacity(), 3u);
}

TEST(SnapshotQueueTest, PushPopSingleThread) {
    SnapshotQueue queue(2);
    CpuSnapshot s1 = makeSnapshot(1);
    CpuSnapshot s2 = makeSnapshot(2);
    queue.pushSnapshot(s1);
    queue.pushSnapshot(s2);
    EXPECT_EQ(queue.size(), 2u);
    auto snap1 = queue.popSnapshot();
    auto snap2 = queue.popSnapshot();
    EXPECT_EQ(snap1.totalUsage, 10);
    EXPECT_EQ(snap1.coresUsage[0], 1);
    EXPECT_EQ(snap2.totalUsage, 20);
    EXPECT_EQ(snap2.coresUsage[0], 2);
    EXPECT_TRUE(queue.empty());
}

TEST(SnapshotQueueTest, PushBlocksWhenFull) {
    SnapshotQueue queue(1);
    CpuSnapshot s1 = makeSnapshot(1);
    CpuSnapshot s2 = makeSnapshot(2);
    queue.pushSnapshot(s1);
    std::atomic<bool> pushed{false};
    std::thread t([&] {
        queue.pushSnapshot(s2);
        pushed = true;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(pushed);
    queue.popSnapshot();
    t.join();
    EXPECT_TRUE(pushed);
}

TEST(SnapshotQueueTest, PopBlocksWhenEmpty) {
    SnapshotQueue queue(1);
    std::atomic<bool> popped{false};
    std::thread t([&] {
        queue.popSnapshot();
        popped = true;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(popped);
    queue.pushSnapshot(makeSnapshot(1));
    t.join();
    EXPECT_TRUE(popped);
}

TEST(SnapshotQueueTest, TimedPopReturnsNulloptOnTimeout) {
    SnapshotQueue queue(1);
    auto result = queue.popSnapshot(std::chrono::milliseconds(100));
    EXPECT_FALSE(result.has_value());
}

TEST(SnapshotQueueTest, TimedPopReturnsValueIfAvailable) {
    SnapshotQueue queue(1);
    queue.pushSnapshot(makeSnapshot(42));
    auto result = queue.popSnapshot(std::chrono::milliseconds(100));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->totalUsage, 42 * 10);
    EXPECT_EQ(result->coresUsage[0], 42);
}
