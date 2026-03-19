#pragma once

#include <SFML/Graphics.hpp>
#include "simulator.hpp"

class Renderer {
public:
    Renderer(unsigned int width = 1000, unsigned int height = 700);

    bool isOpen() const;
    void processEvents();
    void draw(const ElevatorSimulator& simulator);

private:
    float floorY(int floor, int totalFloors) const;
    void drawShaft();
    void drawFloors(const ElevatorSimulator& simulator);
    void drawElevator(const ElevatorSimulator& simulator);

private:
    sf::RenderWindow window_;

    unsigned int width_;
    unsigned int height_;

    float shaftLeft_ = 180.0f;
    float shaftTop_ = 80.0f;
    float shaftWidth_ = 180.0f;
    float shaftHeight_ = 520.0f;
};