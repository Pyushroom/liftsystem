#pragma once

#include "types.hpp"
#include <vector>
#include <optional>
#include <cstddef>

class FloorManager {
public:
    explicit FloorManager(int floors, std::size_t deliveredCapacityPerFloor = 4);

    int floors() const;

    // Add pallet to the waiting queue at source floor
    void addWaitingPallet(const Pallet& pallet);

    // Check if a floor has pallets waiting for pickup
    bool hasWaitingPalletAtFloor(int floor) const;

    // Peek first waiting pallet without removing it
    std::optional<Pallet> peekWaitingPalletAtFloor(int floor) const;

    // Remove first waiting pallet from a floor
    std::optional<Pallet> popWaitingPalletAtFloor(int floor);

    // Delivered/output station handling
    bool canAcceptDeliveredPallet(int floor) const;
    void storeDeliveredPallet(int floor, const Pallet& pallet);

    const std::vector<Pallet>& waitingPalletsAtFloor(int floor) const;
    const std::vector<Pallet>& deliveredPalletsAtFloor(int floor) const;

private:
    struct FloorData {
        std::vector<Pallet> waiting;
        std::vector<Pallet> delivered;
    };

    bool isValidFloor(int floor) const;

private:
    std::vector<FloorData> floors_;
    std::size_t deliveredCapacityPerFloor_;
};