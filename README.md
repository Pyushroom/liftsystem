# Pallet Elevator Control System

C++ project for simulating and controlling an automatic pallet elevator operating between multiple warehouse floors.

## Project goal

The goal of this project is to build a modular control system for a pallet elevator with:

- elevator movement between floors
- pallet transport task handling
- pallet buffering on the elevator platform
- simulation layer for testing without physical hardware
- optional 2D visualization

## Planned architecture

The project will be developed incrementally and will include:

- core domain types
- hardware abstraction layer
- transport task scheduler
- pallet buffer management
- elevator controller based on a finite state machine
- physical simulation model
- visualization layer

## Build

```bash
mkdir build
cd build
cmake ..
make
./pallet_elevator