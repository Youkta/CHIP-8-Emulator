#include "chip8.h"
#include "platform.h"
#include <chrono>
#include <iostream>

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cout << "Usage: " << argv[0]
              << " <num1(scale)> <num2(delay)> <filepath(ROM)>" << std::endl;
    return 1;
  }

  int videoScale = std::stoi(argv[1]);
  int cycleDelay = std::stoi(argv[2]);
  char const *romFilename = argv[3];

  Platform platform("CHIP-8 Emulator", DISPLAY_WIDTH * videoScale,
                    DISPLAY_HEIGHT * videoScale, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  Chip8 chip8;
  chip8.LoadRom(romFilename);

  int videoPitch = sizeof(chip8.display[0]) * DISPLAY_WIDTH;

  auto lastCycleTime = std::chrono::high_resolution_clock::now();
  bool quit = false;

  while (!quit) {
    quit = platform.ProcessInput(chip8.keypad);

    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(
                   currentTime - lastCycleTime)
                   .count();

    if (dt > cycleDelay) {
      lastCycleTime = currentTime;
      std::cout << "cycle\n";
      chip8.Cycle();
      std::cout << "update\n";
      platform.Update(chip8.display, videoPitch);
    }
  }

  return 0;
}
