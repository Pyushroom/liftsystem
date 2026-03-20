#include "controller.hpp"
#include "simulator.hpp"
#include "renderer.hpp"

#include <chrono>
#include <thread>
#include <iostream>

int main() {
    std::cout << "Starting pallet elevator simulation...\n";

    ElevatorSimulator simulator(5);
    ElevatorController controller(simulator, 3);
    Renderer renderer(1000, 700);

    controller.addTask({1, 0, 3});
    controller.addTask({2, 1, 4});
    controller.addTask({3, 2, 0});

    constexpr double dt = 0.1;

    int tick = 0;
    while (renderer.isOpen() && tick < 230) {
        std::cout << "\n=== TICK " << tick << " ===\n";

        renderer.processEvents();

        simulator.setLoadStationReady(true);
        simulator.setUnloadStationReady(true);

        controller.step();
        simulator.update(dt);

        renderer.draw(simulator, controller);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ++tick;
    }

    std::cout << "\nSimulation finished.\n";
    return 0;
}