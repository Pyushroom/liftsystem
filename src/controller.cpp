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
}

// ==========================
// Handlers (empty for now)
// ==========================

void ElevatorController::handleIdle(const Inputs&, Outputs&) {
    // To be implemented
}

void ElevatorController::handleMoving(const Inputs&, Outputs&) {
    // To be implemented
}

void ElevatorController::handleLoading(const Inputs&, Outputs&) {
    // To be implemented
}

void ElevatorController::handleUnloading(const Inputs&, Outputs&) {
    // To be implemented
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