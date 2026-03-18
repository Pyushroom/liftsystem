#pragma once

#include "types.hpp"
#include <vector>
#include <optional>
#include <stdexcept>

// BufferManager represents pallets stored on the elevator platform.
//
// Responsibilities:
// - limit number of pallets (capacity)
// - store pallets currently on the elevator
// - allow loading and unloading pallets
// - find pallet for a specific destination floor
class BufferManager {
public:
    explicit BufferManager(std::size_t capacity);

    // Check if new pallet can be loaded
    bool canLoad() const;

    // Check if buffer is empty
    bool empty() const;

    // Number of pallets on elevator
    std::size_t size() const;

    // Add pallet to buffer
    void loadPallet(const Pallet& pallet);

    // Check if any pallet is destined for given floor
    bool hasPalletForFloor(int floor) const;

    // Remove pallet for given floor (if exists)
    std::optional<Pallet> unloadForFloor(int floor);

    // Access all pallets (read-only)
    const std::vector<Pallet>& pallets() const;

private:
    std::size_t capacity_;
    std::vector<Pallet> pallets_;
};