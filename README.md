# 🚀 Pallet Elevator Control System (C++)

Simulation and control system for an automated pallet elevator operating between multiple floors.

---

## 📌 Overview

This project implements a **modular control system** for a pallet elevator, including:

* finite state machine (FSM) controller
* task scheduler
* pallet buffering system
* hardware abstraction layer
* physical simulation model
* graphical visualization (SFML)
* interactive controls

The system simulates real-world industrial automation logic used in logistics and warehouse systems.

---

## 🧠 Features

### Control System

* Elevator FSM (Idle, Moving, Loading, Unloading, Fault, Emergency)
* Task scheduling for pallet transport
* Buffer management for multiple pallets
* Safety handling (emergency stop, door interlock)

### Simulation

* Continuous physical model (position + velocity)
* Floor detection with tolerance
* Time-based updates (`dt` simulation loop)

### Visualization

* Elevator shaft and floors
* Moving cabin with platform alignment
* Pallets rendered on the platform
* UI panel with system state

### UI / Interaction

* Keyboard controls:

  * `SPACE` – pause / resume
  * `E` – emergency stop
  * `R` – reset emergency stop
  * `T` – add random transport task
  * `ESC` – exit

---

## 🖥️ Demo

The application opens a window showing:

* elevator moving between floors
* pallets being loaded and unloaded
* real-time system state (floor, mode, target, pallet count)

---

## 🏗️ Architecture

```
Controller (FSM)
│
├── Scheduler        (task queue)
├── BufferManager    (pallet storage)
└── Hardware Interface (IElevatorHardware)
        │
        └── Simulator (physical model)
                │
                └── Renderer (SFML UI)
```

---

## ⚙️ Technologies

* C++20
* SFML 3 (graphics)
* CMake
* vcpkg

---

## ▶️ Build & Run

### 1. Configure

```bash
cmake --preset mingw-debug
```

### 2. Build

```bash
cmake --build --preset mingw-debug
```

### 3. Run

```bash
./build/mingw-debug/my_project
```


---

## 💡 What this project demonstrates

* object-oriented system design
* real-time control logic (FSM)
* separation of concerns (logic / hardware / UI)
* simulation of physical processes
* building interactive tools in C++

---

## 📬 Author

Pyushroom
C++ / Embedded / Automation

---
