#pragma once

#include "types.hpp"
#include <queue>
#include <optional>

// Scheduler is responsible for managing transport tasks.
//
// Current implementation:
// - simple FIFO queue

class Scheduler {
public:
    // Add new transport task
    void addTask(const TransportTask& task);

    // Check if there are tasks waiting
    bool hasTask() const;

    // Get current task (front of queue)
    std::optional<TransportTask> currentTask() const;

    // Mark current task as completed
    void completeCurrentTask();

private:
    std::queue<TransportTask> tasks_;
};