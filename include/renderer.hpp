#pragma once

#include <SFML/Graphics.hpp>
#include "simulator.hpp"
#include "controller.hpp"
#include "floor_manager.hpp"

struct InputState {
    bool close = false;
    bool togglePause = false;
    bool emergencyStop = false;
    bool resetEmergency = false;
    bool addTask = false;
};

class Renderer {
public:
    Renderer(unsigned int width = 1000, unsigned int height = 700);

    bool isOpen() const;
    InputState processEvents();
    void draw(const ElevatorSimulator& simulator,
          const ElevatorController& controller,
          const FloorManager& floorManager);

private:
    float floorY(int floor, int totalFloors) const;
    void drawShaft();
    void drawFloors(const ElevatorSimulator& simulator);
    void drawElevator(const ElevatorSimulator& simulator);
    void drawPallets(const ElevatorSimulator& simulator, const ElevatorController& controller);
    void drawTransferPallet(const ElevatorSimulator& simulator, const ElevatorController& controller);
    void drawFloorQueues(const FloorManager& floorManager);
    void drawUI(const ElevatorSimulator& simulator, const ElevatorController& controller);

private:
    sf::RenderWindow window_;
    sf::Font font_;

    unsigned int width_;
    unsigned int height_;

    float shaftLeft_ = 180.0f;
    float shaftTop_ = 80.0f;
    float shaftWidth_ = 180.0f;
    float shaftHeight_ = 520.0f;
};