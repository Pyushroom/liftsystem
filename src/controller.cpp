#include "controller.hpp"
#include <iostream>

ElevatorController::ElevatorController(IElevatorHardware& hardware, std::size_t bufferCapacity)
    : hardware_(hardware),
      buffer_(bufferCapacity) {}

void ElevatorController::addTask(const TransportTask& task) {
    scheduler_.addTask(task);
}

TransferVisual ElevatorController::transferVisual() const {
    return transferVisual_;
}

void ElevatorController::step(double dtSeconds) {
    Inputs in = hardware_.readInputs();

    if (in.emergencyStop) {
        mode_ = ElevatorMode::EmergencyStop;
    } else {
        if (mode_ == ElevatorMode::EmergencyStop) {
            mode_ = ElevatorMode::Idle;
            targetFloor_ = hardware_.currentFloor();
            transferElapsedSeconds_ = 0.0;
            transferVisual_ = {};
        }

        if (in.driveFault) {
            mode_ = ElevatorMode::Fault;
        }
    }

    Outputs out{};

    switch (mode_) {
        case ElevatorMode::Idle:
            handleIdle(in, out);
            break;

        case ElevatorMode::Moving:
            handleMoving(in, out);
            break;

        case ElevatorMode::Loading:
            handleLoading(in, out, dtSeconds);
            break;

        case ElevatorMode::Unloading:
            handleUnloading(in, out, dtSeconds);
            break;

        case ElevatorMode::Fault:
            handleFault(out);
            break;

        case ElevatorMode::EmergencyStop:
            handleEmergency(out);
            break;
    }

    hardware_.applyOutputs(out);
    printStatus();
}

// ==========================
// Handlers 
// ==========================

void ElevatorController::handleIdle(const Inputs&, Outputs&) {
    int currentFloor = hardware_.currentFloor();

    if (buffer_.hasPalletForFloor(currentFloor)) {
        auto pallet = buffer_.peekPalletForFloor(currentFloor);
        if (pallet.has_value()) {
            transferElapsedSeconds_ = 0.0;
            transferVisual_.active = true;
            transferVisual_.loading = false;
            transferVisual_.palletId = pallet->id;
            transferVisual_.sourceFloor = currentFloor;
            transferVisual_.destinationFloor = currentFloor;
            transferVisual_.progress = 0.0;
        }

        mode_ = ElevatorMode::Unloading;
        return;
    }

    auto task = scheduler_.currentTask();
    if (task.has_value()) {
        if (currentFloor == task->sourceFloor && buffer_.canLoad()) {
            transferElapsedSeconds_ = 0.0;
            transferVisual_.active = true;
            transferVisual_.loading = true;
            transferVisual_.palletId = task->palletId;
            transferVisual_.sourceFloor = task->sourceFloor;
            transferVisual_.destinationFloor = task->destinationFloor;
            transferVisual_.progress = 0.0;

            mode_ = ElevatorMode::Loading;
            return;
        }

        if (currentFloor != task->sourceFloor) {
            targetFloor_ = task->sourceFloor;
            mode_ = ElevatorMode::Moving;
            return;
        }
    }

    if (!buffer_.empty()) {
        targetFloor_ = chooseNextTarget(currentFloor);
        if (targetFloor_ != currentFloor) {
            mode_ = ElevatorMode::Moving;
        }
    }
}

void ElevatorController::handleMoving(const Inputs& in, Outputs& out) {
    if (!in.doorClosed) {
        mode_ = ElevatorMode::Fault;
        return;
    }

    // KLUCZOWE: używamy precyzyjnego sprawdzenia
    if (hardware_.isAtFloor(targetFloor_)) {
        out.motorDirection = Direction::Stop;
        out.brakeReleased = false;

        mode_ = ElevatorMode::Idle;
        return;
    }

    out.brakeReleased = true;

    

    int currentFloor = hardware_.currentFloor();

    if (targetFloor_ > currentFloor) {
        out.motorDirection = Direction::Up;
    } else {
        out.motorDirection = Direction::Down;
    }
}

void ElevatorController::handleLoading(const Inputs&, Outputs& out, double dtSeconds) {
    out.doorOpen = true;
    out.conveyorLoad = true;

    auto task = scheduler_.currentTask();
    if (!task.has_value()) {
        transferElapsedSeconds_ = 0.0;
        transferVisual_ = {};
        mode_ = ElevatorMode::Idle;
        return;
    }

    transferElapsedSeconds_ += dtSeconds;
    transferVisual_.progress = std::min(transferElapsedSeconds_ / loadDurationSeconds_, 1.0);

    if (transferElapsedSeconds_ >= loadDurationSeconds_) {
        buffer_.loadPallet(Pallet{
            task->palletId,
            task->sourceFloor,
            task->destinationFloor
        });

        std::cout << "[LOAD] Pallet " << task->palletId
                  << " loaded at floor " << task->sourceFloor
                  << " -> target " << task->destinationFloor << "\n";

        scheduler_.completeCurrentTask();

        transferElapsedSeconds_ = 0.0;
        transferVisual_ = {};
        mode_ = ElevatorMode::Idle;
    }
}

void ElevatorController::handleUnloading(const Inputs&, Outputs& out, double dtSeconds) {
    out.doorOpen = true;
    out.conveyorUnload = true;

    int currentFloor = hardware_.currentFloor();

    transferElapsedSeconds_ += dtSeconds;
    transferVisual_.progress = std::min(transferElapsedSeconds_ / unloadDurationSeconds_, 1.0);

    if (transferElapsedSeconds_ >= unloadDurationSeconds_) {
        auto pallet = buffer_.unloadForFloor(currentFloor);

        if (pallet.has_value()) {
            std::cout << "[UNLOAD] Pallet " << pallet->id
                      << " unloaded at floor " << currentFloor << "\n";
        }

        transferElapsedSeconds_ = 0.0;
        transferVisual_ = {};
        mode_ = ElevatorMode::Idle;
    }
}

void ElevatorController::handleFault(Outputs& out) {
    out.motorDirection = Direction::Stop;
    out.brakeReleased = false;
    out.alarm = true;

    std::cout << "[FAULT] System fault detected\n";
}

void ElevatorController::handleEmergency(Outputs& out) {
    out.motorDirection = Direction::Stop;
    out.brakeReleased = false;
    out.alarm = true;

    std::cout << "[EMERGENCY] Emergency stop active\n";
}

int ElevatorController::chooseNextTarget(int currentFloor) const {
    // Jeśli mamy palety → jedź do pierwszego celu
    if (!buffer_.empty()) {
        for (const auto& pallet : buffer_.pallets()) {
            return pallet.destinationFloor;
        }
    }

    // Jeśli nie, jedź po nowe zadanie
    auto task = scheduler_.currentTask();
    if (task.has_value()) {
        return task->sourceFloor;
    }

    return currentFloor;
}

void ElevatorController::printStatus() const {
    std::cout << "[STATUS] Floor=" << hardware_.currentFloor()
              << " Mode=" << toString(mode_)
              << " Target=" << targetFloor_
              << " Buffer=" << buffer_.size()
              << "\n";
}

const BufferManager& ElevatorController::buffer() const {
    return buffer_;
}

ElevatorMode ElevatorController::mode() const {
    return mode_;
}

int ElevatorController::targetFloor() const {
    return targetFloor_;
}