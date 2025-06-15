#pragma once

#include "common.hpp"

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace mp {
namespace cpuMonitor {

class SnapshotQueue {
public:
    explicit SnapshotQueue(size_t capacity);
    void pushSnapshot(const CpuSnapshot &snapshot);
    CpuSnapshot popSnapshot();
    std::optional<CpuSnapshot> popSnapshot(std::chrono::milliseconds timeout);
    bool empty() const;
    size_t capacity() const { return capacity_; }
    size_t size() const;

private:
    mutable std::mutex mtx_;
    std::condition_variable cvNotEmpty_;
    std::condition_variable cvNotFull_;
    std::queue<CpuSnapshot> queue_;
    size_t capacity_;
};

} // namespace cpuMonitor
} // namespace mp
