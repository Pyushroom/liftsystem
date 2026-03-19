#pragma once

#include "hardware.hpp"

class ElevatorSimulator : public IElevatorHardware {
public:
    explicit ElevatorSimulator(int floors);

    Inputs readInputs() override;
    int currentFloor() const override;
    bool isAtFloor(int floor) const override;
    void applyOutputs(const Outputs& outputs) override;

    void update();

    void setLoadStationReady(bool value);
    void setUnloadStationReady(bool value);
    void triggerEmergencyStop(bool value);
    void triggerDriveFault(bool value);

private:
    int floors_;
    int currentFloor_ = 0;

    Inputs inputs_{};
    Outputs outputs_{};
};