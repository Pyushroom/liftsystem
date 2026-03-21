#include "renderer.hpp"
#include <optional>

Renderer::Renderer(unsigned int width, unsigned int height)
    : window_(sf::VideoMode(sf::Vector2u(width, height)), "Pallet Elevator Simulation"),
      width_(width),
      height_(height) {
    window_.setFramerateLimit(60);

    if (!font_.openFromFile("assets/arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }
}

bool Renderer::isOpen() const {
    return window_.isOpen();
}

InputState Renderer::processEvents() {
    InputState input{};

    while (const std::optional event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            input.close = true;
            window_.close();
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Escape:
                    input.close = true;
                    window_.close();
                    break;

                case sf::Keyboard::Key::Space:
                    input.togglePause = true;
                    break;

                case sf::Keyboard::Key::E:
                    input.emergencyStop = true;
                    break;

                case sf::Keyboard::Key::R:
                    input.resetEmergency = true;
                    break;

                case sf::Keyboard::Key::T:
                    input.addTask = true;
                    break;

                default:
                    break;
            }
        }
    }

    return input;
}

void Renderer::draw(const ElevatorSimulator& simulator, const ElevatorController& controller) {
    window_.clear(sf::Color(30, 30, 35));

    drawShaft();
    drawFloors(simulator);
    drawElevator(simulator);
    drawPallets(simulator, controller);
    drawTransferPallet(simulator, controller);
    drawUI(simulator, controller);

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

void Renderer::drawPallets(const ElevatorSimulator& simulator,
                           const ElevatorController& controller) {
    const auto& pallets = controller.buffer().pallets();
    const TransferVisual transfer = controller.transferVisual();

    const float cabinWidth = 110.0f;
    const float cabinHeight = 60.0f;
    const float platformOffsetY = cabinHeight - 12.0f;
    const float cabinX = shaftLeft_ + (shaftWidth_ - cabinWidth) / 2.0f;

    const int floors = simulator.floors();

    const float yFloor0 = floorY(0, floors);
    const float yTopFloor = floorY(floors - 1, floors);
    const float maxPhysicalPos = static_cast<float>((floors - 1) * 3.0);

    float normalized = 0.0f;
    if (maxPhysicalPos > 0.0f) {
        normalized = static_cast<float>(simulator.positionMeters()) / maxPhysicalPos;
    }

    const float platformSurfaceY = yFloor0 - normalized * (yFloor0 - yTopFloor);
    const float cabinY = platformSurfaceY - platformOffsetY;

    int visibleIndex = 0;
    for (const auto& pallet : pallets) {
        if (transfer.active && !transfer.loading && pallet.id == transfer.palletId) {
            continue;
        }

        sf::RectangleShape rect(sf::Vector2f(28.0f, 18.0f));

        float offsetX = 12.0f + static_cast<float>(visibleIndex) * 32.0f;
        float offsetY = -18.0f;

        rect.setPosition(sf::Vector2f(
            cabinX + offsetX,
            cabinY + platformOffsetY + offsetY
        ));

        rect.setFillColor(sf::Color(160, 110, 60));
        rect.setOutlineThickness(1.0f);
        rect.setOutlineColor(sf::Color::Black);

        window_.draw(rect);
        ++visibleIndex;
    }
}

void Renderer::drawUI(const ElevatorSimulator& simulator,
                      const ElevatorController& controller) {

    sf::RectangleShape panel(sf::Vector2f(280.0f, shaftHeight_));
    panel.setPosition(sf::Vector2f(450.0f, shaftTop_));
    panel.setFillColor(sf::Color(40, 40, 48));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(150, 150, 150));
    window_.draw(panel);

    const int floor = simulator.currentFloor();
    const int target = controller.targetFloor();
    const int pallets = static_cast<int>(controller.buffer().size());

    const ElevatorMode mode = controller.mode();
    const std::string modeStr = toString(mode);

    sf::Color stateColor = sf::Color::White;
    switch (mode) {
        case ElevatorMode::Idle:
            stateColor = sf::Color(60, 170, 90);
            break;
        case ElevatorMode::Moving:
            stateColor = sf::Color(80, 120, 220);
            break;
        case ElevatorMode::Loading:
        case ElevatorMode::Unloading:
            stateColor = sf::Color(220, 190, 80);
            break;
        case ElevatorMode::Fault:
        case ElevatorMode::EmergencyStop:
            stateColor = sf::Color(220, 70, 70);
            break;
    }

    float y = shaftTop_ + 20.0f;

    auto drawText = [&](const std::string& label, sf::Color color = sf::Color::White) {
        sf::Text text(font_);
        text.setString(label);
        text.setCharacterSize(18);
        text.setFillColor(color);
        text.setPosition(sf::Vector2f(470.0f, y));

        window_.draw(text);
        y += 30.0f;
    };

    drawText("Elevator Status");
    y += 10.0f;

    // kolorowy wskaźnik stanu
    sf::CircleShape indicator(10.0f);
    indicator.setPosition(sf::Vector2f(470.0f, y + 4.0f));
    indicator.setFillColor(stateColor);
    window_.draw(indicator);

    sf::Text modeText(font_);
    modeText.setString("Mode: " + modeStr);
    modeText.setCharacterSize(18);
    modeText.setFillColor(stateColor);
    modeText.setPosition(sf::Vector2f(500.0f, y));
    window_.draw(modeText);

    y += 40.0f;

    drawText("Floor: " + std::to_string(floor));
    drawText("Target: " + std::to_string(target));
    drawText("Pallets: " + std::to_string(pallets));
}

void Renderer::drawTransferPallet(const ElevatorSimulator& simulator,
                                  const ElevatorController& controller) {
    const TransferVisual transfer = controller.transferVisual();
    if (!transfer.active) {
        return;
    }

    const int floors = simulator.floors();
    const float currentFloorY = floorY(simulator.currentFloor(), floors);

    const float cabinWidth = 110.0f;
    const float cabinHeight = 60.0f;
    const float platformOffsetY = cabinHeight - 12.0f;

    const float cabinX = shaftLeft_ + (shaftWidth_ - cabinWidth) / 2.0f;

    const float yFloor0 = floorY(0, floors);
    const float yTopFloor = floorY(floors - 1, floors);
    const float maxPhysicalPos = static_cast<float>((floors - 1) * 3.0);

    float normalized = 0.0f;
    if (maxPhysicalPos > 0.0f) {
        normalized = static_cast<float>(simulator.positionMeters()) / maxPhysicalPos;
    }

    const float platformSurfaceY = yFloor0 - normalized * (yFloor0 - yTopFloor);
    const float cabinY = platformSurfaceY - platformOffsetY;

    const float stationStartX = 430.0f;
    const float stationY = currentFloorY - 18.0f;

    const float platformX = cabinX + 12.0f;
    const float platformY = cabinY + platformOffsetY - 18.0f;

    const float p = static_cast<float>(transfer.progress);

    float x = 0.0f;
    float y = 0.0f;

    if (transfer.loading) {
        x = stationStartX + (platformX - stationStartX) * p;
        y = stationY + (platformY - stationY) * p;
    } else {
        x = platformX + (stationStartX - platformX) * p;
        y = platformY + (stationY - platformY) * p;
    }

    sf::RectangleShape rect(sf::Vector2f(28.0f, 18.0f));
    rect.setPosition(sf::Vector2f(x, y));
    rect.setFillColor(sf::Color(190, 130, 70));
    rect.setOutlineThickness(2.0f);
    rect.setOutlineColor(sf::Color::Black);
    window_.draw(rect);
}