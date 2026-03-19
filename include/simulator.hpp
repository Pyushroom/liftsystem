#pragma once

#include "hardware.hpp"

// Simulator with simple physical model:
//
// - continuous position (meters)
// - constant speed movement
// - mapping position -> floor
class ElevatorSimulator : public IElevatorHardware {
public:
    explicit ElevatorSimulator(int floors);

    // IElevatorHardware
    Inputs readInputs() override;
    int currentFloor() const override;
    bool isAtFloor(int floor) const override;
    void applyOutputs(const Outputs& outputs) override;

    // Simulation step
    void update(double dtSeconds);

    // Control inputs
    void setLoadStationReady(bool value);
    void setUnloadStationReady(bool value);
    void triggerEmergencyStop(bool value);
    void triggerDriveFault(bool value);

    // Debug / visualization
    double positionMeters() const;
    int floors() const;

private:
    double floorToPosition(int floor) const;
    int positionToFloor(double position) const;

private:
    int floors_;

    double floorHeight_ = 3.0;          // meters
    double speed_ = 1.5;                // m/s

    double position_ = 0.0;             // current position
    double velocity_ = 0.0;             // current velocity

    Inputs inputs_{};
    Outputs outputs_{};
};