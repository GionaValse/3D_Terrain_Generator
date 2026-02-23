# 3D Terrain Generator (Perlin Noise)

A procedural 3D terrain generation tool developed in **C++17**. This project leverages the **Perlin Noise** algorithm to create realistic, coherent heightmaps suitable for environmental simulations or game development.

---

## 🏛 Academic Context
This project was developed for **SUPSI DTI** (Department of Innovative Technologies), in collaboration with **Pietro Brusadelli**.

---

## 🚀 Key Features
* **Procedural Generation:** Generate unique, infinite-feeling landscapes based on noise coordinates.
* **Perlin Noise Engine:** Powered by the header-only [Reputeless/PerlinNoise](https://github.com/Reputeless/PerlinNoise) library (C++17/C++20).
* **Cross-Platform Build System:** A robust Makefile compatible with Windows (MinGW), macOS, and Linux.
* **Modular Architecture:** Clean separation between core logic (`src`), headers (`include`), and unit testing (`tests`).

---

## 🛠️ Requirements
* **Compiler:** GCC/G++ with **C++17** support or higher.
* **Build Tool:** GNU Make (or MinGW-make for Windows users).

---

## 📂 Project Structure

```text
.
├── bin/            # Compiled executables
├── build/          # Intermediate object files (.o)
├── include/        # Header files (.hpp, .h)
├── src/            # Application source code (.cpp)
├── tests/          # Unit test suite
└── Makefile        # Automated build and run script
```

---

## 💻 Getting Started

### 1. Setup
Ensure you have the `PerlinNoise.hpp` header file placed inside the `include/` directory.

### 2. Compile and Run
To build the project and launch the application immediately, use the following command:

```bash
make run
```

---

### 3. Other Commands
* **Compile only:** `make all`
* **Execute Tests:** `make test`
* **Clean Workspace:** `make clean`

---

## ⚙️ How It Works
The generator maps `x` and `y` grid coordinates into coherent noise values. By layering different frequencies and amplitudes, the system simulates natural features like rolling hills or jagged mountains.

The fundamental height `z` calculation follows the principle:

```text
z = perlin(x * frequency, y * frequency) * amplitude
```

---

## 📜 Credits & Acknowledgments
* **Authors:** Developed at **SUPSI DTI** in collaboration with **Pietro Brusadelli**.
* **Library:** Uses the `PerlinNoise` header by [Ryo Suzuki (Reputeless)](https://github.com/Reputeless).