#include "controller.hpp"
#include "simulator.hpp"

#include <chrono>
#include <thread>
#include <iostream>

int main() {
    std::cout << "Starting pallet elevator simulation...\n";

    // 1. Create simulator (hardware layer)
    ElevatorSimulator simulator(5);

    // 2. Create controller
    ElevatorController controller(simulator, 3);

    // 3. Add transport tasks
    controller.addTask({1, 0, 3});
    controller.addTask({2, 1, 4});
    controller.addTask({3, 2, 0});

    constexpr double dt = 0.1; // 100 ms

    // 4. Simulation loop
    for (int tick = 0; tick < 100; ++tick) {
        std::cout << "\n=== TICK " << tick << " ===\n";

        // Simulate stations ready
        simulator.setLoadStationReady(true);
        simulator.setUnloadStationReady(true);

        // Run controller logic
        controller.step();

        // Update simulation (movement)
        simulator.update(dt);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\nSimulation finished.\n";

    return 0;
}