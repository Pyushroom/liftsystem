#include "buffer_manager.hpp"

BufferManager::BufferManager(std::size_t capacity)
    : capacity_(capacity) {}

bool BufferManager::canLoad() const {
    return pallets_.size() < capacity_;
}

bool BufferManager::empty() const {
    return pallets_.empty();
}

std::size_t BufferManager::size() const {
    return pallets_.size();
}

void BufferManager::loadPallet(const Pallet& pallet) {
    if (!canLoad()) {
        throw std::runtime_error("Elevator buffer is full");
    }

    pallets_.push_back(pallet);
}

bool BufferManager::hasPalletForFloor(int floor) const {
    for (const auto& pallet : pallets_) {
        if (pallet.destinationFloor == floor) {
            return true;
        }
    }
    return false;
}

std::optional<Pallet> BufferManager::unloadForFloor(int floor) {
    for (auto it = pallets_.begin(); it != pallets_.end(); ++it) {
        if (it->destinationFloor == floor) {
            Pallet pallet = *it;
            pallets_.erase(it);
            return pallet;
        }
    }

    return std::nullopt;
}

const std::vector<Pallet>& BufferManager::pallets() const {
    return pallets_;
}