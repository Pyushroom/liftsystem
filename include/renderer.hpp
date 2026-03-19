#pragma once

#include <SFML/Graphics.hpp>

class Renderer {
public:
    Renderer(unsigned int width = 1000, unsigned int height = 700);

    bool isOpen() const;
    void processEvents();
    void draw();

private:
    sf::RenderWindow window_;
};