#pragma once

#include <chrono>
#include <cstdint>
#include <random>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;
const unsigned int DISPLAY_WIDTH = 64;
const unsigned int DISPLAY_HEIGHT = 32;

class Chip8 {
  // Parameters
public:
  uint8_t registers[16];
  uint8_t memory[4096];
  uint16_t index;
  uint16_t pc;
  uint16_t stack[16];
  uint8_t sp;
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t keypad[16];
  uint32_t display[64 * 32];
  uint16_t opcode;

private:
  std::default_random_engine randGen;
  std::uniform_int_distribution<uint8_t> randByte;

  // Methods
public:
  Chip8();
  void LoadRom(char const *filename);
  void Cycle();

  // OpCodes
private:
  void Table0();
  void Table8();
  void TableE();
  void TableF();

  void Op_00E0();
  void Op_00EE();
  void Op_1nnn();
  void Op_2nnn();
  void Op_3xkk();
  void Op_4xkk();
  void Op_5xy0();
  void Op_6xkk();
  void Op_7xkk();
  void Op_8xy0();
  void Op_8xy1();
  void Op_8xy2();
  void Op_8xy3();
  void Op_8xy4();
  void Op_8xy5();
  void Op_8xy6();
  void Op_8xy7();
  void Op_8xyE();
  void Op_9xy0();
  void Op_Annn();
  void Op_Bnnn();
  void Op_Cxkk();
  void Op_Dxyn();
  void Op_Ex9E();
  void Op_ExA1();
  void Op_Fx07();
  void Op_Fx0A();
  void Op_Fx15();
  void Op_Fx18();
  void Op_Fx1E();
  void Op_Fx29();
  void Op_Fx33();
  void Op_Fx55();
  void Op_Fx65();
  void Op_NULL();

  typedef void (Chip8::*Chip8Func)();
  Chip8Func table[0xF + 1];
  Chip8Func table0[0xE + 1];
  Chip8Func table8[0xE + 1];
  Chip8Func tableE[0xE + 1];
  Chip8Func tableF[0x65 + 1];
};
