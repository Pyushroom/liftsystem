#include "controller.hpp"
#include <iostream>

ElevatorController::ElevatorController(IElevatorHardware& hardware, std::size_t bufferCapacity)
    : hardware_(hardware),
      buffer_(bufferCapacity) {}

void ElevatorController::addTask(const TransportTask& task) {
    scheduler_.addTask(task);
}

void ElevatorController::step() {
    // 1. Read inputs
    Inputs in = hardware_.readInputs();

    // 2. Global safety checks
    if (in.emergencyStop) {
        mode_ = ElevatorMode::EmergencyStop;
    } else if (in.driveFault) {
        mode_ = ElevatorMode::Fault;
    }

    // 3. Prepare outputs
    Outputs out{};

    // 4. State machine
    switch (mode_) {
        case ElevatorMode::Idle:
            handleIdle(in, out);
            break;

        case ElevatorMode::Moving:
            handleMoving(in, out);
            break;

        case ElevatorMode::Loading:
            handleLoading(in, out);
            break;

        case ElevatorMode::Unloading:
            handleUnloading(in, out);
            break;

        case ElevatorMode::Fault:
            handleFault(out);
            break;

        case ElevatorMode::EmergencyStop:
            handleEmergency(out);
            break;
    }

    // 5. Apply outputs
    hardware_.applyOutputs(out);
    printStatus();
}

// ==========================
// Handlers 
// ==========================

void ElevatorController::handleIdle(const Inputs&, Outputs&) {
    int currentFloor = hardware_.currentFloor();

    // 1. Najpierw rozładuj to co już jest na windzie
    if (buffer_.hasPalletForFloor(currentFloor)) {
        mode_ = ElevatorMode::Unloading;
        return;
    }

    // 2. Obsłuż nowe zadanie (załadunek)
    auto task = scheduler_.currentTask();
    if (task.has_value()) {
        if (currentFloor == task->sourceFloor && buffer_.canLoad()) {
            mode_ = ElevatorMode::Loading;
            return;
        }

        if (currentFloor != task->sourceFloor) {
            targetFloor_ = task->sourceFloor;
            mode_ = ElevatorMode::Moving;
            return;
        }
    }

    // 3. Jeśli mamy palety, ale nie na tym piętrze → jedź je rozładować
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

    // Jeśli jesteśmy na celu → stop
    if (hardware_.isAtFloor(targetFloor_)) {
        out.motorDirection = Direction::Stop;
        out.brakeReleased = false;

        mode_ = ElevatorMode::Idle;
        return;
    }

    int currentFloor = hardware_.currentFloor();

    out.brakeReleased = true;

    if (targetFloor_ > currentFloor) {
        out.motorDirection = Direction::Up;
    } else {
        out.motorDirection = Direction::Down;
    }
}

void ElevatorController::handleLoading(const Inputs& in, Outputs& out) {
    out.doorOpen = true;
    out.conveyorLoad = true;

    auto task = scheduler_.currentTask();
    if (!task.has_value()) {
        mode_ = ElevatorMode::Idle;
        return;
    }

    // Symulacja: jeśli stacja gotowa → załaduj paletę
    if (in.loadStationReady && buffer_.canLoad()) {
        buffer_.loadPallet(Pallet{
            task->palletId,
            task->sourceFloor,
            task->destinationFloor
        });

        std::cout << "[LOAD] Pallet " << task->palletId
                  << " loaded at floor " << task->sourceFloor
                  << " -> target " << task->destinationFloor << "\n";

        scheduler_.completeCurrentTask();

        mode_ = ElevatorMode::Idle;
    }
}

void ElevatorController::handleUnloading(const Inputs& in, Outputs& out) {
    out.doorOpen = true;
    out.conveyorUnload = true;

    int currentFloor = hardware_.currentFloor();

    if (in.unloadStationReady) {
        auto pallet = buffer_.unloadForFloor(currentFloor);

        if (pallet.has_value()) {
            std::cout << "[UNLOAD] Pallet " << pallet->id
                      << " unloaded at floor " << currentFloor << "\n";
        }

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