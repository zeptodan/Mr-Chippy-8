# Mr-Chippy-8

A CHIP-8 emulator written in modern C++ with support for CHIP-8, Super-CHIP, and XO-CHIP. The project aims to provide accurate emulation while maintaining a clean and extensible codebase.

Developed and tested on Linux (WSL) using SDL2 for rendering/audio, Dear ImGui for the user interface, and Native File Dialog (NFD) for ROM loading.

---

## Features

### CHIP-8

* Complete CHIP-8 instruction set
* Configurable execution speed
* Accurate timers
* Sound support
* Keyboard input handling

### Super-CHIP

* High-resolution mode (128×64)
* Scrolling instructions
* 16×16 sprites
* RPL user flags
* Super-CHIP compatibility features

### XO-CHIP

* Dual bitplane graphics
* 4-color rendering using two bitplanes
* XO-CHIP audio pattern playback
* Extended memory support
* Long memory addressing (`F000`)
* Extended save/load instructions (`5XY2`, `5XY3`)
* Full XO-CHIP instruction set support

### User Interface

* Dear ImGui-based menu system
* Native file picker integration through NFD
* ROM loading from the application
* Runtime configuration options

---

## Accuracy

The emulator has been validated using CHIP-8, Super-CHIP, and XO-CHIP test ROMs.

All implemented instruction sets pass the test ROMs used during development.

---

## Dependencies

The project uses:

* SDL2
* Dear ImGui
* Native File Dialog Extended (NFD-Extended)

ImGui and NFD are included as vendored dependencies inside the repository and are built automatically through CMake.

---

## Build Requirements

* GCC 13 or newer
* CMake
* C++17 compatible compiler
* SDL2 development libraries

### Ubuntu / Debian

```bash
sudo apt install build-essential cmake libsdl2-dev
```

---

## Building

Clone the repository:

```bash
git clone https://github.com/zeptodan/Mr-Chippy-8.git
cd Mr-Chippy-8
```

Configure:

```bash
cmake -B build
```

Build:

```bash
cmake --build build
```

Run:

```bash
./build/Mr-Chippy-8
```

---

## Project Structure

```text
Mr-Chippy-8/
├── src/
│   ├── chip8/
│   ├── graphics/
│   ├── audio/
│   └── ...
├── third-party/
│   ├── imgui/
│   └── nfd-extended/
├── roms/
├── CMakeLists.txt
└── README.md
```

(The exact structure may evolve as the project grows.)

---

## Graphics

The emulator supports:

| System     | Resolution     |
| ---------- | -------------- |
| CHIP-8     | 64×32          |
| Super-CHIP | 64×32 / 128×64 |
| XO-CHIP    | 64×32 / 128×64 |

XO-CHIP graphics are implemented using two independent bitplanes, allowing four visible colors through bitplane combinations.

---

## Audio

XO-CHIP audio pattern playback is implemented in addition to the traditional CHIP-8 buzzer.

Audio output is generated through SDL2.

---

## Development Notes

The emulator was developed as a learning project focused on:

* Modern C++
* Emulator architecture
* Instruction decoding and execution
* Graphics programming with SDL2
* Audio generation
* CMake-based project organization
* Cross-platform application development

The codebase is designed to remain readable and approachable while supporting the full CHIP-8 family of systems.

---

## Future Improvements

Potential future additions include:

* Additional debugging tools
* Memory and register inspection windows
* Disassembler view
* Breakpoint support
* Save states
* Windows-specific build configuration
* Additional UI customization

---

## Acknowledgements

* CHIP-8 community documentation
* Octo and XO-CHIP documentation
* SDL2
* Dear ImGui
* Native File Dialog Extended
* CHIP-8 test ROM authors and maintainers
