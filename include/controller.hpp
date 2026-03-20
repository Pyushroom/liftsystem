#pragma once

#include "hardware.hpp"
#include "scheduler.hpp"
#include "buffer_manager.hpp"
#include <cstddef>

// ElevatorController is the main control unit of the system.
//
// Responsibilities:
// - read inputs from hardware
// - process logic (state machine)
// - generate outputs
// - coordinate scheduler and buffer
class ElevatorController {
public:
    ElevatorController(IElevatorHardware& hardware, std::size_t bufferCapacity);

    // Add transport task to scheduler
    void addTask(const TransportTask& task);

    // Main control loop step (called periodically)
    void step();

    const BufferManager& buffer() const;

    ElevatorMode mode() const;
    int targetFloor() const;

private:
    // Internal states of controller
    enum class PendingAction {
        None,
        Load,
        Unload
    };

    // State handlers (to be implemented in next commits)
    void handleIdle(const Inputs& in, Outputs& out);
    void handleMoving(const Inputs& in, Outputs& out);
    void handleLoading(const Inputs& in, Outputs& out);
    void handleUnloading(const Inputs& in, Outputs& out);
    void handleFault(Outputs& out);
    void handleEmergency(Outputs& out);

    int chooseNextTarget(int currentFloor) const;
    void printStatus() const;

private:
    IElevatorHardware& hardware_;
    Scheduler scheduler_;
    BufferManager buffer_;

    ElevatorMode mode_ = ElevatorMode::Idle;
    PendingAction pendingAction_ = PendingAction::None;

    int targetFloor_ = 0;
};