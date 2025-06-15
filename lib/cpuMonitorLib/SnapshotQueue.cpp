#include "SnapshotQueue.hpp"
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace mp {
namespace cpuMonitor {

namespace {
// Helper to pop a snapshot from the queue
CpuSnapshot popFront(std::queue<CpuSnapshot> &queue) {
    CpuSnapshot snapshot = std::move(queue.front());
    queue.pop();
    return snapshot;
}
} // anonymous namespace

SnapshotQueue::SnapshotQueue(size_t capacity) : capacity_(capacity) {}

void SnapshotQueue::pushSnapshot(const CpuSnapshot &snapshot) {
    std::unique_lock<std::mutex> lock(mtx_);
    cvNotFull_.wait(lock, [this] { return queue_.size() < capacity_; });
    queue_.push(snapshot);
    cvNotEmpty_.notify_one();
}

CpuSnapshot SnapshotQueue::popSnapshot() {
    std::unique_lock<std::mutex> lock(mtx_);
    cvNotEmpty_.wait(lock, [this] { return !queue_.empty(); });
    auto snap = popFront(queue_);
    cvNotFull_.notify_one();
    return snap;
}

std::optional<CpuSnapshot> SnapshotQueue::popSnapshot(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mtx_);
    if (!cvNotEmpty_.wait_for(lock, timeout, [this] { return !queue_.empty(); })) {
        return std::nullopt; // Timeout occurred
    }
    auto snap = popFront(queue_);
    cvNotFull_.notify_one();
    return snap;
}

size_t SnapshotQueue::size() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return queue_.size();
}

bool SnapshotQueue::empty() const { return size() == 0; }

} // namespace cpuMonitor
} // namespace mp
