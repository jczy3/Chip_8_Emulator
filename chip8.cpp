#include "chip8.h"
#include <iostream>
using std::cout;
#define VX V[opcode & 0x0F00 >> 8] 
#define VY V[opcode & 0x00F0 >> 4] 
#define NN opcode & 0x00FF
#define NNN opcode & 0x0FFF
#define VF V[0xF]

chip8::chip8()
{
}

chip8::~chip8()
{
}

void chip8::initalize()
{
  pc     = 0x200; // Program counter starts at 0x200
  opcode = 0;     // Reset current opcode
  I      = 0;     // Reset index register
  sp     = 0;     // Reset stack pointer

  for (int i{}; i < 80; ++i)
    memory[i] = chip8_fontset[i];
}

void chip8::emulateCycle()
{
  // Fetch Opcode
  opcode = memory[pc] << 8 | memory[pc + 1];

  // Decode/Execute Opcode
  switch (opcode & 0xF000)
  {
    case 0x0000:
      switch(opcode & 0x000F)
      {
        case 0x00E0:    // Clear the screen

        break;

        case 0x00EE:    // Return from subroutine
          pc = stack[sp];
          --sp;
        break;

        default:
          cout << "Unknown opcode [0x0000]: 0x" << opcode << "\n";
      }
    break;

    case 0x1000:        // Jump to address NNN
      pc = NNN;
    break;
      
    case 0x2000:        // Call subroutine at NNN
      stack[sp] = pc;
      ++sp;
      pc = NNN;
    break;

    case 0x3000:        // (3XNN) Skips next instruction if VX == NN
      if (VX == NN)
        pc += 4;
    break;

    case 0x4000:        // (4XNN) Skips next instruction if VX != NN
      if (VX != NN)
        pc += 4;
    break;

    case 0x5000:        // (5XY0) Skips next instruction if VX == VY
      if (VX == VY)
        pc += 4;
    break;

    case 0x6000:        // (6XNN) Sets VX = NN
      VX = NN;
      pc += 2;
    break;

    case 0x7000:        // (7XNN) Adds NN to VX (carry flag unchanged)
      VX += NN;
      pc += 2;
    break;

    case 0x8000:
      switch (opcode & 0x000F){
        case 0x0000:    // (8XY0) Set VX = VY
          VX = VY;
          pc += 2;
        break;

        case 0x0001:    // (8XY1) Set VX |= VY
          VX |= VY;
          pc += 2;
        break;

        case 0x0002:    // (8XY2) Set VX &= VY
          VX &= VY;
          pc += 2;
        break;

        case 0x0003:    // (8XY3) Set VX ^= VY
          VX ^= VY;
          pc += 2;
        break;

        case 0x0004:    // (8XY4) VX += VY
          if (VY > 0xFF - VX)     // Overflow
            VF = 1;
          else 
            VF = 0;
          VX += VY;
          pc += 2;
        break;

        case 0x0005:    // (8XY5) VX -= VY
          if (VX > 0x00 + VY)     // No underflow
            VF = 1;
          else  
            VF = 0;
          VX -= VY;
          pc += 2;
        break;

        case 0x0006:    // (8XY6) Right shift VX
          VF = VX & 0x0001;
          VX >>= 1;
          pc += 2;
        break;

        case 0x0007:    // (8XY7) VX = VY - VX
          VX = VY - VX;     // Fix this, it is wrong
          pc += 2;
        break;

        case 0x000E:    // (8XYE) Left shift VX
          if (VX & 0x8000)
            VF = 1;
          else
            VF = 0;
          VX <<= 1;
          pc += 2;
        break;
      }
    break;
  }

  // Update Timers
  if (delay_timer > 0)
    --delay_timer;
  
  if (sound_timer > 0)
    --sound_timer;
}

