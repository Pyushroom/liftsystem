#include "simulator.hpp"
#include <iostream>

ElevatorSimulator::ElevatorSimulator(int floors)
    : floors_(floors) {}

Inputs ElevatorSimulator::readInputs() {
    return inputs_;
}

int ElevatorSimulator::currentFloor() const {
    return currentFloor_;
}

bool ElevatorSimulator::isAtFloor(int floor) const {
    return currentFloor_ == floor;
}

void ElevatorSimulator::applyOutputs(const Outputs& outputs) {
    outputs_ = outputs;
}

void ElevatorSimulator::update() {
    // Symulacja ruchu między piętrami (krokowo)
    if (outputs_.motorDirection == Direction::Up && currentFloor_ < floors_ - 1) {
        currentFloor_++;
    }
    else if (outputs_.motorDirection == Direction::Down && currentFloor_ > 0) {
        currentFloor_--;
    }

    // Log stanu (debug)
    std::cout << "[SIM] Floor=" << currentFloor_
              << " Motor=" << toString(outputs_.motorDirection)
              << " Door=" << (outputs_.doorOpen ? "OPEN" : "CLOSED")
              << " Load=" << (outputs_.conveyorLoad ? "ON" : "OFF")
              << " Unload=" << (outputs_.conveyorUnload ? "ON" : "OFF")
              << "\n";
}

void ElevatorSimulator::setLoadStationReady(bool value) {
    inputs_.loadStationReady = value;
}

void ElevatorSimulator::setUnloadStationReady(bool value) {
    inputs_.unloadStationReady = value;
}

void ElevatorSimulator::triggerEmergencyStop(bool value) {
    inputs_.emergencyStop = value;
}

void ElevatorSimulator::triggerDriveFault(bool value) {
    inputs_.driveFault = value;
}