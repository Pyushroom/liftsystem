#pragma once

#include "hardware.hpp"
#include "scheduler.hpp"
#include "buffer_manager.hpp"
#include <cstddef>
#include <optional>

struct TransferVisual {
    bool active = false;
    bool loading = false;
    int palletId = -1;
    int sourceFloor = 0;
    int destinationFloor = 0;
    double progress = 0.0;
};

class ElevatorController {
public:
    ElevatorController(IElevatorHardware& hardware, std::size_t bufferCapacity);

    void addTask(const TransportTask& task);
    void step(double dtSeconds);

    const BufferManager& buffer() const;
    ElevatorMode mode() const;
    int targetFloor() const;
    TransferVisual transferVisual() const;

private:
    enum class PendingAction {
        None,
        Load,
        Unload
    };

    void handleIdle(const Inputs& in, Outputs& out);
    void handleMoving(const Inputs& in, Outputs& out);
    void handleLoading(const Inputs& in, Outputs& out, double dtSeconds);
    void handleUnloading(const Inputs& in, Outputs& out, double dtSeconds);
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

    double transferElapsedSeconds_ = 0.0;
    double loadDurationSeconds_ = 2.0;
    double unloadDurationSeconds_ = 2.0;

    TransferVisual transferVisual_{};
};