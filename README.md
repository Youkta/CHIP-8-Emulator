# CHIP-8 Emulator

A C++ implementation of the classic CHIP-8 virtual machine and emulator, featuring CPU instruction execution, memory management, graphics rendering, keyboard input, and ROM execution using SDL2 and GLAD.

## Overview

CHIP-8 is a simple interpreted virtual machine originally designed for running small games and programs. This project implements the core CHIP-8 architecture in C++ and provides a desktop interface for executing CHIP-8 ROMs.

The emulator reproduces the main components of the CHIP-8 system:

* 4 KB memory
* 16 general-purpose 8-bit registers (`V0`–`VF`)
* 16-bit index register (`I`)
* Program counter (`PC`)
* Stack and stack pointer
* Delay and sound timers
* 64 × 32 monochrome display
* Hexadecimal keypad input
* CHIP-8 opcode instruction set

## Architecture

```text
                 CHIP-8 Emulator
                       │
          ┌────────────┴────────────┐
          │                         │
       CHIP-8 VM                Platform Layer
          │                         │
   ┌──────┼──────┐            ┌─────┴─────┐
   │      │      │            │           │
 Memory  CPU   Timers       SDL2        GLAD
   │      │      │            │           │
   └──────┴──────┘            ├───────────┤
          │                   │           │
       Display             Graphics     Input
          │                   │           │
          └───────────────────┴───────────┘
                       │
                    CHIP-8 ROM
```

## Execution Cycle

The emulator repeatedly performs the standard fetch-decode-execute cycle:

```text
        ┌──────────────┐
        │ Fetch Opcode │
        └──────┬───────┘
               ↓
        ┌──────────────┐
        │ Decode       │
        │ Instruction  │
        └──────┬───────┘
               ↓
        ┌──────────────┐
        │ Execute      │
        │ Opcode       │
        └──────┬───────┘
               ↓
        ┌──────────────┐
        │ Update PC /  │
        │ VM State     │
        └──────┬───────┘
               │
               └──────────→ Fetch
```

## Core Components

### CPU & Instruction Set

The emulator implements CHIP-8 opcode decoding and execution, including:

* Register operations
* Arithmetic and logical operations
* Conditional branching
* Subroutine calls and returns
* Stack operations
* Memory load/store instructions
* Random number generation
* Sprite drawing
* Keyboard-dependent instructions

### Memory

The virtual machine provides a 4 KB address space. Programs are loaded into the appropriate memory region and accessed through the program counter and index register.

### Display

The CHIP-8 display is represented as a 64 × 32 monochrome framebuffer. Sprite instructions update the framebuffer using XOR-based drawing and collision detection.

### Input

CHIP-8 uses a hexadecimal keypad containing 16 keys. Keyboard events from SDL2 are mapped to the corresponding CHIP-8 keys.

### Timers

The emulator maintains the CHIP-8 delay and sound timers and updates them according to the VM timing requirements.

## Project Structure

```text
CHIP8-Emulator/
│
├── chip8.cpp          # CHIP-8 virtual machine implementation
├── chip8.h            # VM class and architecture definitions
├── platform.cpp       # SDL2 graphics and input handling
├── platform.h         # Platform interface
├── main.cpp           # Emulator entry point
│
├── include/           # External headers
├── lib/               # External libraries
└── test-roms/         # CHIP-8 test ROMs
```

## Technologies

* **C++**
* **SDL2**
* **GLAD**
* CHIP-8 virtual machine architecture
* Low-level CPU emulation
* Event-driven keyboard input
* 2D framebuffer rendering

## ROM Execution

The emulator supports loading CHIP-8 ROMs and executing them through the implemented virtual machine. Test ROMs can be used to validate instruction execution, graphics rendering, keyboard input, and overall emulator behaviour.

## Build & Run

Compile the emulator with:

```bash
g++ main.cpp chip8.cpp platform.cpp -o chip8 -I ./include -L ./lib -l SDL2 -l glad
```

Then launch it with:

```bash
./chip8
```

## Validation

The implementation was tested using CHIP-8 ROMs to verify:

* Opcode execution
* Register and memory operations
* Control-flow instructions
* Sprite rendering
* Collision behaviour
* Keyboard input
* Overall VM execution

## Learning Outcomes

This project provides hands-on experience with:

* CPU emulation and instruction decoding
* Virtual machine architecture
* Memory and register modelling
* Fetch-decode-execute cycles
* Low-level C++ programming
* Event-driven input handling
* Real-time 2D rendering

## Future Scope

Potential extensions include:

* Sound generation
* Interactive debugger interface
* Opcode tracing and debugging tools
* Additional ROM compatibility testing

## References

Implementation developed following the CHIP-8 emulator architecture and concepts described in Austin Morlan's CHIP-8 emulator tutorial.
