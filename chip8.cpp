#include "chip8.h"
#include <fstream>
#include <iostream>

uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
  // Set PC
  pc = START_ADDRESS;

  // Initialize RNG
  randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

  // Load fontset
  for (unsigned int i = 0; i < FONTSET_SIZE; i++) {
    memory[FONTSET_START_ADDRESS + i] = fontset[i];
  }

  // Setup function pointer table
  table[0x0] = &Chip8::Table0;
  table[0x1] = &Chip8::Op_1nnn;
  table[0x2] = &Chip8::Op_2nnn;
  table[0x3] = &Chip8::Op_3xkk;
  table[0x4] = &Chip8::Op_4xkk;
  table[0x5] = &Chip8::Op_5xy0;
  table[0x6] = &Chip8::Op_6xkk;
  table[0x7] = &Chip8::Op_7xkk;
  table[0x8] = &Chip8::Table8;
  table[0x9] = &Chip8::Op_9xy0;
  table[0xA] = &Chip8::Op_Annn;
  table[0xB] = &Chip8::Op_Bnnn;
  table[0xC] = &Chip8::Op_Cxkk;
  table[0xD] = &Chip8::Op_Dxyn;
  table[0xE] = &Chip8::TableE;
  table[0xF] = &Chip8::TableF;

  for (size_t i = 0; i <= 0xE; i++) {
    table0[i] = &Chip8::Op_NULL;
    table8[i] = &Chip8::Op_NULL;
    tableE[i] = &Chip8::Op_NULL;
  }

  table0[0x0] = &Chip8::Op_00E0;
  table0[0xE] = &Chip8::Op_00EE;

  table8[0x0] = &Chip8::Op_8xy0;
  table8[0x1] = &Chip8::Op_8xy1;
  table8[0x2] = &Chip8::Op_8xy2;
  table8[0x3] = &Chip8::Op_8xy3;
  table8[0x4] = &Chip8::Op_8xy4;
  table8[0x5] = &Chip8::Op_8xy5;
  table8[0x6] = &Chip8::Op_8xy6;
  table8[0x7] = &Chip8::Op_8xy7;
  table8[0xE] = &Chip8::Op_8xyE;

  tableE[0x1] = &Chip8::Op_ExA1;
  tableE[0xE] = &Chip8::Op_Ex9E;

  for (size_t i = 0; i <= 0x65; i++) {
    tableF[i] = &Chip8::Op_NULL;
  }

  tableF[0x07] = &Chip8::Op_Fx07;
  tableF[0x0A] = &Chip8::Op_Fx0A;
  tableF[0x15] = &Chip8::Op_Fx15;
  tableF[0x18] = &Chip8::Op_Fx18;
  tableF[0x1E] = &Chip8::Op_Fx1E;
  tableF[0x29] = &Chip8::Op_Fx29;
  tableF[0x33] = &Chip8::Op_Fx33;
  tableF[0x55] = &Chip8::Op_Fx55;
  tableF[0x65] = &Chip8::Op_Fx65;
}

void Chip8::LoadRom(const char *filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (file.is_open()) {
    std::streampos size = file.tellg();
    char *buffer = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    for (long i = 0; i < size; i++) {
      memory[START_ADDRESS + i] = buffer[i];
    }

    delete[] buffer;
  }
}

void Chip8::Cycle() {
  std::cerr << '1';
  opcode = (memory[pc] << 8 | memory[pc + 1]);
  pc += 2;
  std::cerr << '2';
  ((*this).*(table[(opcode & 0xF000u) >> 12u]))();
  std::cerr << '3' << std::endl;
  if (delay_timer > 0)
    delay_timer--;
  if (sound_timer > 0)
    sound_timer--;
}

// OpCode implmentations
void Chip8::Op_00E0() {
  // Clear the screen
  memset(display, 0, sizeof(display));
}

void Chip8::Op_00EE() {
  // Return from subroutine
  sp--;
  pc = stack[sp];
}

void Chip8::Op_1nnn() {
  // Jump to location nnn
  uint16_t address = opcode & 0x0FFFu;
  pc = address;
}

void Chip8::Op_2nnn() {
  // Call subroutine at nnn
  uint16_t address = opcode & 0x0FFFu;

  stack[sp] = pc;
  sp++;
  pc = address;
}

void Chip8::Op_3xkk() {
  // Skip next instruction if Vx = kk
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;

  if (registers[Vx] == byte) {
    pc += 2;
  }
}

void Chip8::Op_4xkk() {
  // Skip next instruction if Vx != kk
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;

  if (registers[Vx] != byte) {
    pc += 2;
  }
}

void Chip8::Op_5xy0() {
  // Skip next instruction if Vx = Vy
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  if (registers[Vx] == registers[Vy]) {
    pc += 2;
  }
}

void Chip8::Op_6xkk() {
  // Set Vx = kk
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;

  registers[Vx] = byte;
}

void Chip8::Op_7xkk() {
  // Set Vx = Vx + kk
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;

  registers[Vx] += byte;
}

void Chip8::Op_8xy0() {
  // Set Vx = Vy
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] = registers[Vy];
}

void Chip8::Op_8xy1() {
  // Set Vx = Vx OR Vy
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] |= registers[Vy];
}

void Chip8::Op_8xy2() {
  // Set Vx = Vx AND Vy
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] &= registers[Vy];
}

void Chip8::Op_8xy3() {
  // Set Vx = Vx XOR Vy
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  registers[Vx] ^= registers[Vy];
}

void Chip8::Op_8xy4() {
  // Set Vx = Vx + Vy, set VF = carry
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  uint16_t sum = registers[Vx] + registers[Vy];

  (sum > 255U) ? registers[0xF] = 1 : registers[0xF] = 0;

  registers[Vx] = sum & 0xFFu;
}

void Chip8::Op_8xy5() {
  // Set Vx = Vx - Vy, set VF = NOT borrow
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  (registers[Vx] > registers[Vy]) ? registers[0xF] = 1 : registers[0xF] = 0;

  registers[Vx] -= registers[Vy];
}

void Chip8::Op_8xy6() {
  // Set Vx = Vx SHR 1
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  registers[0xF] = registers[Vx] & 0x1u;
  registers[Vx] >>= 1;
}

void Chip8::Op_8xy7() {
  // Set Vx = Vy - Vx, set VF = NOT borrow
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  (registers[Vy] > registers[Vx]) ? registers[0xF] = 1 : registers[0xF] = 0;

  registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::Op_8xyE() {
  // Set Vx = Vx SHL 1
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
  registers[Vx] <<= 1;
}

void Chip8::Op_9xy0() {
  // Skip next instruction if Vx != Vy
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;

  if (registers[Vx] != registers[Vy]) {
    pc += 2;
  }
}

void Chip8::Op_Annn() {
  // Set I = nnn
  uint16_t address = opcode & 0x0FFFu;

  index = address;
}

void Chip8::Op_Bnnn() {
  // Jump to location nnn + V0
  uint16_t address = opcode & 0x0FFFu;

  pc = registers[0] + address;
}

void Chip8::Op_Cxkk() {
  // Set Vx = random byte AND kk
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;

  registers[Vx] = randByte(randGen) & byte;
}

void Chip8::Op_Dxyn() {
  // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF =
  // collision
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  uint8_t height = opcode & 0x000Fu;

  uint8_t xPos = registers[Vx] % DISPLAY_WIDTH;
  uint8_t yPos = registers[Vy] % DISPLAY_HEIGHT;

  registers[0xF] = 0;

  for (unsigned int row = 0; row < height; ++row) {
    uint8_t spriteByte = memory[index + row];

    for (unsigned int col = 0; col < 8; ++col) {
      uint8_t spritePixel = spriteByte & (0x80u >> col);
      uint32_t *screenPixel =
          &display[(yPos + row) * DISPLAY_WIDTH + (xPos + col)];

      if (spritePixel) {
        if (*screenPixel == 0xFFFFFFFF) {
          registers[0xF] = 1;
        }

        *screenPixel ^= 0xFFFFFFFF;
      }
    }
  }
}

void Chip8::Op_Ex9E() {
  // Skip next instruction if key with the value of Vx is pressed
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t key = registers[Vx];

  if (keypad[key]) {
    pc += 2;
  }
}

void Chip8::Op_ExA1() {
  // Skip next instruction if key with the value of Vx is not pressed
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t key = registers[Vx];

  if (!keypad[key]) {
    pc += 2;
  }
}

void Chip8::Op_Fx07() {
  // Set Vx = delay timer value
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  registers[Vx] = delay_timer;
}

void Chip8::Op_Fx0A() {
  // Wait for a key press, store the value of the key in Vx
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  for (unsigned int i = 0; i < 16; i++) {
    if (keypad[i]) {
      registers[Vx] = i;
      return;
    }
  }
  pc -= 2;
}

void Chip8::Op_Fx15() {
  // Set delay timer = Vx
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  delay_timer = registers[Vx];
}

void Chip8::Op_Fx18() {
  // Set sound timer = Vx
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  sound_timer = registers[Vx];
}

void Chip8::Op_Fx1E() {
  // Set I = I + Vx
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  index += registers[Vx];
}

void Chip8::Op_Fx29() {
  // Set I = location of sprite for digit Vx
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t digit = registers[Vx];

  index = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::Op_Fx33() {
  // Store BCD representation of Vx in memory locations I, I+1, and I+2
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t value = registers[Vx];

  memory[index + 2] = value % 10;
  value /= 10;
  memory[index + 1] = value % 10;
  value /= 10;
  memory[index] = value % 10;
}

void Chip8::Op_Fx55() {
  // Store registers V0 through Vx in memory starting at location I
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  for (unsigned int i = 0; i <= Vx; i++) {
    memory[index + i] = registers[i];
  }
}

void Chip8::Op_Fx65() {
  // Read registers V0 through Vx from memory starting at location I
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;

  for (unsigned int i = 0; i <= Vx; i++) {
    registers[i] = memory[index + i];
  }
}

void Chip8::Op_NULL() {
  // Do nothing
}

void Chip8::Table0() { ((*this).*(table0[opcode & 0x000Fu]))(); }
void Chip8::Table8() { ((*this).*(table8[opcode & 0x000Fu]))(); }
void Chip8::TableE() { ((*this).*(tableE[opcode & 0x000Fu]))(); }
void Chip8::TableF() { ((*this).*(tableF[opcode & 0x00FFu]))(); }
