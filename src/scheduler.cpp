#include "scheduler.hpp"

void Scheduler::addTask(const TransportTask& task) {
    tasks_.push(task);
}

bool Scheduler::hasTask() const {
    return !tasks_.empty();
}

std::optional<TransportTask> Scheduler::currentTask() const {
    if (tasks_.empty()) {
        return std::nullopt;
    }
    return tasks_.front();
}

void Scheduler::completeCurrentTask() {
    if (!tasks_.empty()) {
        tasks_.pop();
    }
}