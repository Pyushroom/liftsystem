#include "controller.hpp"
#include "simulator.hpp"
#include "renderer.hpp"

#include <chrono>
#include <thread>
#include <iostream>

int main() {
    std::cout << "Starting pallet elevator simulation...\n";

    ElevatorSimulator simulator(5);
    FloorManager floorManager(5);
    ElevatorController controller(simulator, floorManager, 3);
    Renderer renderer(1000, 700);

    floorManager.addWaitingPallet({1, 0, 3});
    floorManager.addWaitingPallet({2, 1, 4});
    floorManager.addWaitingPallet({3, 2, 0});

    constexpr double dt = 0.1;

    bool paused = false;
    int nextTaskId = 100;

    int tick = 0;
    while (renderer.isOpen() && tick < 2000) {
        std::cout << "\n=== TICK " << tick << " ===\n";

        InputState input = renderer.processEvents();

        if (input.togglePause) {
            paused = !paused;
        }

        if (input.emergencyStop) {
            simulator.triggerEmergencyStop(true);
        }

        if (input.resetEmergency) {
            simulator.triggerEmergencyStop(false);
        }

        if (input.addTask) {
            int src = std::rand() % simulator.floors();
            int dst = std::rand() % simulator.floors();

            if (src != dst) {
                floorManager.addWaitingPallet({nextTaskId++, src, dst});
            }
        }

        simulator.setLoadStationReady(true);
        simulator.setUnloadStationReady(true);

        if (!paused) {
            controller.step(dt);
            simulator.update(dt);
        }

        renderer.draw(simulator, controller, floorManager);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ++tick;
    }

    std::cout << "\nSimulation finished.\n";
    return 0;
}