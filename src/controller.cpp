#include "controller.hpp"
#include <iostream>

ElevatorController::ElevatorController(IElevatorHardware& hardware,
                                       FloorManager& floorManager,
                                       std::size_t bufferCapacity)
    : hardware_(hardware),
      floorManager_(floorManager),
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
    const int currentFloor = hardware_.currentFloor();

    // 1. Jeśli mamy paletę do rozładunku na tym piętrze
    if (buffer_.hasPalletForFloor(currentFloor)) {
        auto pallet = buffer_.peekPalletForFloor(currentFloor);
        if (pallet.has_value()) {
            transferElapsedSeconds_ = 0.0;
            transferVisual_ = {
                true,
                false,
                pallet->id,
                currentFloor,
                currentFloor,
                0.0
            };
        }

        mode_ = ElevatorMode::Unloading;
        return;
    }

    // 2. Jeśli na tym piętrze czeka paleta i mamy miejsce
    if (floorManager_.hasWaitingPalletAtFloor(currentFloor) && buffer_.canLoad()) {
        auto pallet = floorManager_.peekWaitingPalletAtFloor(currentFloor);

        if (pallet.has_value()) {
            transferElapsedSeconds_ = 0.0;
            transferVisual_ = {
                true,
                true,
                pallet->id,
                pallet->sourceFloor,
                pallet->destinationFloor,
                0.0
            };

            mode_ = ElevatorMode::Loading;
            return;
        }
    }

    // 3. Jeśli są jeszcze palety do dostarczenia albo odebrania, wybierz cel
    const int target = chooseNextTarget(currentFloor);
    if (target != currentFloor) {
        targetFloor_ = target;
        mode_ = ElevatorMode::Moving;
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

    transferElapsedSeconds_ += dtSeconds;
    transferVisual_.progress = std::min(transferElapsedSeconds_ / loadDurationSeconds_, 1.0);

    if (transferElapsedSeconds_ >= loadDurationSeconds_) {
        auto pallet = floorManager_.popWaitingPalletAtFloor(hardware_.currentFloor());

        if (!pallet.has_value()) {
            transferElapsedSeconds_ = 0.0;
            transferVisual_ = {};
            mode_ = ElevatorMode::Idle;
            return;
        }

        buffer_.loadPallet(*pallet);

        std::cout << "[LOAD] Pallet " << pallet->id
                  << " loaded at floor " << pallet->sourceFloor
                  << " -> target " << pallet->destinationFloor << "\n";

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
            if (floorManager_.canAcceptDeliveredPallet(currentFloor)) {
                floorManager_.storeDeliveredPallet(currentFloor, *pallet);

                std::cout << "[UNLOAD] Pallet " << pallet->id
                        << " delivered at floor " << currentFloor << "\n";
            }
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
    // 1. Najpierw dostarcz to, co już jest na windzie
    if (!buffer_.empty()) {
        for (const auto& pallet : buffer_.pallets()) {
            return pallet.destinationFloor;
        }
    }

    // 2. Potem jedź po najbliższą oczekującą paletę
    auto nextWaitingFloor = floorManager_.findNextFloorWithWaitingPallet();
    if (nextWaitingFloor.has_value()) {
        return *nextWaitingFloor;
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