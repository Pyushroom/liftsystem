#pragma once
#include <string>

// =====================================
// Basic motion definitions
// =====================================

enum class Direction {
    Up,
    Down,
    Stop
};

// =====================================
// Elevator high-level states
// =====================================

enum class ElevatorMode {
    Idle,
    Moving,
    Loading,
    Unloading,
    Fault,
    EmergencyStop
};

// =====================================
// Domain models
// =====================================

struct Pallet {
    int id;
    int sourceFloor;
    int destinationFloor;
};

struct TransportTask {
    int palletId;
    int sourceFloor;
    int destinationFloor;
};

// =====================================
// Simulated hardware inputs
// =====================================

struct Inputs {
    bool emergencyStop = false;
    bool driveFault = false;

    bool doorClosed = true;

    // stations
    bool loadStationReady = false;
    bool unloadStationReady = false;
};

// =====================================
// Simulated hardware outputs
// =====================================

struct Outputs {
    Direction motorDirection = Direction::Stop;
    bool brakeReleased = false;

    bool doorOpen = false;

    bool conveyorLoad = false;
    bool conveyorUnload = false;

    bool alarm = false;
};

// =====================================
// Helpers for debugging / logging
// =====================================

inline std::string toString(Direction dir) {
    switch (dir) {
        case Direction::Up: return "UP";
        case Direction::Down: return "DOWN";
        case Direction::Stop: return "STOP";
    }
    return "UNKNOWN";
}

inline std::string toString(ElevatorMode mode) {
    switch (mode) {
        case ElevatorMode::Idle: return "Idle";
        case ElevatorMode::Moving: return "Moving";
        case ElevatorMode::Loading: return "Loading";
        case ElevatorMode::Unloading: return "Unloading";
        case ElevatorMode::Fault: return "Fault";
        case ElevatorMode::EmergencyStop: return "EmergencyStop";
    }
    return "Unknown";
}