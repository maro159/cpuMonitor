#pragma once

#include "common.hpp"

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace mp {
namespace cpuMonitor {

/**
 * @brief Thread-safe queue for CPU usage snapshots.
 */
class SnapshotQueue {
public:
    /**
     * @brief Construct a new SnapshotQueue with a given capacity.
     * @param capacity Maximum number of snapshots in the queue.
     */
    explicit SnapshotQueue(size_t capacity);
    /**
     * @brief Push a snapshot into the queue. Will block if the queue is full.
     * @param snapshot The CPU usage snapshot to push.
     */
    void pushSnapshot(const CpuSnapshot &snapshot);
    
    /**
     * @brief Pop a snapshot from the queue (blocking). Will block until a snapshot is available.
     * @return The next CPU usage snapshot.
     */
    CpuSnapshot popSnapshot();
    /**
     * @brief Pop a snapshot from the queue with timeout.
     * @param timeout Maximum time to wait for a snapshot.
     * @return Optional snapshot if available, std::nullopt on timeout.
     */
    std::optional<CpuSnapshot> popSnapshot(std::chrono::milliseconds timeout);
    /**
     * @brief Check if the queue is empty.
     * @return True if empty, false otherwise.
     */
    bool empty() const;
    /**
     * @brief Get the queue capacity.
     * @return The maximum number of snapshots.
     */
    size_t capacity() const { return capacity_; }
    /**
     * @brief Get the current queue size.
     * @return The number of snapshots in the queue.
     */
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
