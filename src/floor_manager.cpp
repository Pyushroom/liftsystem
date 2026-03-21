#include "floor_manager.hpp"
#include <stdexcept>

FloorManager::FloorManager(int floors, std::size_t deliveredCapacityPerFloor)
    : floors_(static_cast<std::size_t>(floors)),
      deliveredCapacityPerFloor_(deliveredCapacityPerFloor) {
    if (floors <= 0) {
        throw std::invalid_argument("Number of floors must be positive");
    }
}

int FloorManager::floors() const {
    return static_cast<int>(floors_.size());
}

void FloorManager::addWaitingPallet(const Pallet& pallet) {
    if (!isValidFloor(pallet.sourceFloor)) {
        throw std::out_of_range("Invalid source floor");
    }

    floors_[static_cast<std::size_t>(pallet.sourceFloor)].waiting.push_back(pallet);
}

bool FloorManager::hasWaitingPalletAtFloor(int floor) const {
    if (!isValidFloor(floor)) {
        return false;
    }

    return !floors_[static_cast<std::size_t>(floor)].waiting.empty();
}

std::optional<Pallet> FloorManager::peekWaitingPalletAtFloor(int floor) const {
    if (!isValidFloor(floor)) {
        return std::nullopt;
    }

    const auto& waiting = floors_[static_cast<std::size_t>(floor)].waiting;
    if (waiting.empty()) {
        return std::nullopt;
    }

    return waiting.front();
}

std::optional<Pallet> FloorManager::popWaitingPalletAtFloor(int floor) {
    if (!isValidFloor(floor)) {
        return std::nullopt;
    }

    auto& waiting = floors_[static_cast<std::size_t>(floor)].waiting;
    if (waiting.empty()) {
        return std::nullopt;
    }

    const Pallet pallet = waiting.front();
    waiting.erase(waiting.begin());
    return pallet;
}

std::optional<int> FloorManager::findNextFloorWithWaitingPallet() const {
    for (std::size_t i = 0; i < floors_.size(); ++i) {
        if (!floors_[i].waiting.empty()) {
            return static_cast<int>(i);
        }
    }
    return std::nullopt;
}

bool FloorManager::canAcceptDeliveredPallet(int floor) const {
    if (!isValidFloor(floor)) {
        return false;
    }

    const auto& delivered = floors_[static_cast<std::size_t>(floor)].delivered;
    return delivered.size() < deliveredCapacityPerFloor_;
}

void FloorManager::storeDeliveredPallet(int floor, const Pallet& pallet) {
    if (!isValidFloor(floor)) {
        throw std::out_of_range("Invalid destination floor");
    }

    auto& delivered = floors_[static_cast<std::size_t>(floor)].delivered;
    if (delivered.size() >= deliveredCapacityPerFloor_) {
        throw std::runtime_error("Delivered pallet buffer is full");
    }

    delivered.push_back(pallet);
}

const std::vector<Pallet>& FloorManager::waitingPalletsAtFloor(int floor) const {
    if (!isValidFloor(floor)) {
        throw std::out_of_range("Invalid floor");
    }

    return floors_[static_cast<std::size_t>(floor)].waiting;
}

const std::vector<Pallet>& FloorManager::deliveredPalletsAtFloor(int floor) const {
    if (!isValidFloor(floor)) {
        throw std::out_of_range("Invalid floor");
    }

    return floors_[static_cast<std::size_t>(floor)].delivered;
}

bool FloorManager::isValidFloor(int floor) const {
    return floor >= 0 && static_cast<std::size_t>(floor) < floors_.size();
}