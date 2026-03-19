#include "renderer.hpp"
#include <optional>

Renderer::Renderer(unsigned int width, unsigned int height)
    : window_(sf::VideoMode(sf::Vector2u(width, height)), "Pallet Elevator Simulation") {
    window_.setFramerateLimit(60);
}

bool Renderer::isOpen() const {
    return window_.isOpen();
}

void Renderer::processEvents() {
    while (const std::optional event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        }
    }
}

void Renderer::draw() {
    window_.clear(sf::Color(30, 30, 35));
    window_.display();
}