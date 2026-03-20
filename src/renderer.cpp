#include "renderer.hpp"
#include <optional>

Renderer::Renderer(unsigned int width, unsigned int height)
    : window_(sf::VideoMode(sf::Vector2u(width, height)), "Pallet Elevator Simulation"),
      width_(width),
      height_(height) {
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

void Renderer::draw(const ElevatorSimulator& simulator) {
    window_.clear(sf::Color(30, 30, 35));

    drawShaft();
    drawFloors(simulator);
    drawElevator(simulator);
    drawPallets(simulator);

    window_.display();
}

float Renderer::floorY(int floor, int totalFloors) const {
    const float step = shaftHeight_ / static_cast<float>(totalFloors - 1);
    return shaftTop_ + shaftHeight_ - step * static_cast<float>(floor);
}

void Renderer::drawShaft() {
    sf::RectangleShape shaft(sf::Vector2f(shaftWidth_, shaftHeight_));
    shaft.setPosition(sf::Vector2f(shaftLeft_, shaftTop_));
    shaft.setFillColor(sf::Color(50, 50, 58));
    shaft.setOutlineThickness(3.0f);
    shaft.setOutlineColor(sf::Color(180, 180, 180));
    window_.draw(shaft);

    sf::RectangleShape guideLeft(sf::Vector2f(6.0f, shaftHeight_));
    guideLeft.setPosition(sf::Vector2f(shaftLeft_ + 25.0f, shaftTop_));
    guideLeft.setFillColor(sf::Color(100, 100, 110));
    window_.draw(guideLeft);

    sf::RectangleShape guideRight(sf::Vector2f(6.0f, shaftHeight_));
    guideRight.setPosition(sf::Vector2f(shaftLeft_ + shaftWidth_ - 31.0f, shaftTop_));
    guideRight.setFillColor(sf::Color(100, 100, 110));
    window_.draw(guideRight);
}

void Renderer::drawFloors(const ElevatorSimulator& simulator) {
    const int floors = simulator.floors();

    for (int floor = 0; floor < floors; ++floor) {
        const float y = floorY(floor, floors);

        sf::RectangleShape line(sf::Vector2f(420.0f, 2.0f));
        line.setPosition(sf::Vector2f(60.0f, y));
        line.setFillColor(sf::Color(200, 200, 200));
        window_.draw(line);

        sf::RectangleShape station(sf::Vector2f(120.0f, 24.0f));
        station.setPosition(sf::Vector2f(390.0f, y - 24.0f));
        station.setFillColor(sf::Color(90, 90, 100));
        station.setOutlineThickness(1.0f);
        station.setOutlineColor(sf::Color(170, 170, 170));
        window_.draw(station);
    }
}

void Renderer::drawElevator(const ElevatorSimulator& simulator) {
    const float cabinWidth = 110.0f;
    const float cabinHeight = 60.0f;
    const float platformHeight = 10.0f;
    const float platformOffsetX = 5.0f;
    const float platformOffsetY = cabinHeight - 12.0f;

    const int floors = simulator.floors();

    // Pozycja piętra 0 i najwyższego piętra w układzie renderera
    const float yFloor0 = floorY(0, floors);
    const float yTopFloor = floorY(floors - 1, floors);

    // Zakres fizyczny ruchu
    const float maxPhysicalPos = static_cast<float>((floors - 1) * 3.0);

    float normalized = 0.0f;
    if (maxPhysicalPos > 0.0f) {
        normalized = static_cast<float>(simulator.positionMeters()) / maxPhysicalPos;
    }

    // Aktualna wysokość platformy windy w pikselach:
    // platforma ma dokładnie trafiać w linię piętra
    const float platformSurfaceY = yFloor0 - normalized * (yFloor0 - yTopFloor);

    const float cabinX = shaftLeft_ + (shaftWidth_ - cabinWidth) / 2.0f;

    // Kabina jest rysowana tak, żeby platforma była na platformSurfaceY
    const float cabinY = platformSurfaceY - platformOffsetY;

    sf::RectangleShape cabin(sf::Vector2f(cabinWidth, cabinHeight));
    cabin.setPosition(sf::Vector2f(cabinX, cabinY));
    cabin.setFillColor(sf::Color(60, 170, 90));
    cabin.setOutlineThickness(3.0f);
    cabin.setOutlineColor(sf::Color::White);
    window_.draw(cabin);

    sf::RectangleShape platform(sf::Vector2f(cabinWidth - 10.0f, platformHeight));
    platform.setPosition(sf::Vector2f(cabinX + platformOffsetX, cabinY + platformOffsetY));
    platform.setFillColor(sf::Color(70, 70, 70));
    window_.draw(platform);
}

void Renderer::drawPallets(const ElevatorSimulator& simulator) {
    const int palletCount = static_cast<int>(simulator.currentFloor()); // TEMP (zaraz poprawimy)

    // Na razie: fake liczba palet żeby zobaczyć efekt
    // Docelowo podłączymy BufferManager

    const float cabinWidth = 110.0f;
    const float cabinHeight = 60.0f;

    const float platformOffsetX = 5.0f;
    const float platformOffsetY = cabinHeight - 12.0f;

    const float cabinX = shaftLeft_ + (shaftWidth_ - cabinWidth) / 2.0f;

    const int floors = simulator.floors();

    const float yFloor0 = floorY(0, floors);
    const float yTopFloor = floorY(floors - 1, floors);

    const float maxPhysicalPos =
        static_cast<float>((floors - 1) * 3.0);

    float normalized = 0.0f;
    if (maxPhysicalPos > 0.0f) {
        normalized = static_cast<float>(simulator.positionMeters()) / maxPhysicalPos;
    }

    const float platformSurfaceY = yFloor0 - normalized * (yFloor0 - yTopFloor);

    const float cabinY = platformSurfaceY - platformOffsetY;

    // Rysujemy kilka "palet"
    const int maxPallets = 3;

    for (int i = 0; i < maxPallets; ++i) {
        sf::RectangleShape pallet(sf::Vector2f(28.0f, 18.0f));

        float offsetX = 12.0f + i * 32.0f;
        float offsetY = -18.0f;

        pallet.setPosition(sf::Vector2f(
            cabinX + offsetX,
            cabinY + platformOffsetY + offsetY
        ));

        pallet.setFillColor(sf::Color(160, 110, 60));
        pallet.setOutlineThickness(1.0f);
        pallet.setOutlineColor(sf::Color::Black);

        window_.draw(pallet);
    }
}