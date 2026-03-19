#include "simulator.hpp"
#include <iostream>
#include <cmath>

ElevatorSimulator::ElevatorSimulator(int floors)
    : floors_(floors) {}

Inputs ElevatorSimulator::readInputs() {
    return inputs_;
}

int ElevatorSimulator::currentFloor() const {
    return positionToFloor(position_);
}

bool ElevatorSimulator::isAtFloor(int floor) const {
    double target = floorToPosition(floor);
    return std::abs(position_ - target) < 0.05; // tolerance
}

void ElevatorSimulator::applyOutputs(const Outputs& outputs) {
    outputs_ = outputs;
}

void ElevatorSimulator::update(double dtSeconds) {
    // Safety: stop if fault or E-stop
    if (inputs_.emergencyStop || inputs_.driveFault) {
        velocity_ = 0.0;
    }
    else if (!inputs_.doorClosed) {
        velocity_ = 0.0;
    }
    else {
        switch (outputs_.motorDirection) {
            case Direction::Up:
                velocity_ = speed_;
                break;
            case Direction::Down:
                velocity_ = -speed_;
                break;
            case Direction::Stop:
                velocity_ = 0.0;
                break;
        }
    }

    // Integrate position
    position_ += velocity_ * dtSeconds;

    // Clamp limits
    double minPos = 0.0;
    double maxPos = floorToPosition(floors_ - 1);

    if (position_ < minPos) {
        position_ = minPos;
        velocity_ = 0.0;
    }

    if (position_ > maxPos) {
        position_ = maxPos;
        velocity_ = 0.0;
    }

    // Snap to floor if stopped
    for (int f = 0; f < floors_; ++f) {
        if (isAtFloor(f) && outputs_.motorDirection == Direction::Stop) {
            position_ = floorToPosition(f);
            break;
        }
    }

    // Update door sensor
    inputs_.doorClosed = !outputs_.doorOpen;

    // Debug log
    std::cout << "[SIM] pos=" << position_
              << " floor=" << currentFloor()
              << " motor=" << toString(outputs_.motorDirection)
              << " door=" << (outputs_.doorOpen ? "OPEN" : "CLOSED")
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

double ElevatorSimulator::positionMeters() const {
    return position_;
}

double ElevatorSimulator::floorToPosition(int floor) const {
    return static_cast<double>(floor) * floorHeight_;
}

int ElevatorSimulator::positionToFloor(double position) const {
    int f = static_cast<int>(std::round(position / floorHeight_));
    if (f < 0) f = 0;
    if (f >= floors_) f = floors_ - 1;
    return f;
}