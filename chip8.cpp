#include "chip8.h"
#include <iostream>
#include <cstring>
using std::cout;
#define VX V[(opcode & 0x0F00) >> 8]
#define VY V[(opcode & 0x00F0) >> 4]
#define NN (opcode & 0x00FF)
#define NNN (opcode & 0x0FFF)
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

  memset(gfx, 0, sizeof(gfx));
  memset(memory, 0, sizeof(memory));
  memset(V, 0, sizeof(V));
  memset(stack, 0, sizeof(stack));
  memset(key, 0, sizeof(key));

  drawFlag = false;

  unsigned char fontset[80] = {
    0xF0,0x90,0x90,0x90,0xF0, // 0
    0x20,0x60,0x20,0x20,0x70, // 1
    0xF0,0x10,0xF0,0x80,0xF0, // 2
    0xF0,0x10,0xF0,0x10,0xF0, // 3
    0x90,0x90,0xF0,0x10,0x10, // 4
    0xF0,0x80,0xF0,0x10,0xF0, // 5
    0xF0,0x80,0xF0,0x90,0xF0, // 6
    0xF0,0x10,0x20,0x40,0x40, // 7
    0xF0,0x90,0xF0,0x90,0xF0, // 8
    0xF0,0x90,0xF0,0x10,0xF0, // 9
    0xF0,0x90,0xF0,0x90,0x90, // A
    0xE0,0x90,0xE0,0x90,0xE0, // B
    0xF0,0x80,0x80,0x80,0xF0, // C
    0xE0,0x90,0x90,0x90,0xE0, // D
    0xF0,0x80,0xF0,0x80,0xF0, // E
    0xF0,0x80,0xF0,0x80,0x80  // F
  };
  for (int i = 0; i < 80; ++i)
    memory[i] = fontset[i];
}

void chip8::emulateCycle()
{
  // Fetch Opcode
  opcode = memory[pc] << 8 | memory[pc + 1];

  // Decode/Execute Opcode
  switch (opcode & 0xF000)
  {
    case 0x0000:
      switch(opcode & 0x00FF)
      {
        case 0x00E0:    // Clear the screen
          memset(gfx, 0, sizeof(gfx));
          drawFlag = true;
          pc += 2;
        break;

        case 0x00EE:    // Return from subroutine
          --sp;
          pc = stack[sp];
          pc += 2;
        break;

        default:
          cout << "Unknown opcode [0x0000]: 0x" << opcode << "\n";
          pc += 2;
          break;
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
      else
        pc += 2;
    break;

    case 0x4000:        // (4XNN) Skips next instruction if VX != NN
      if (VX != NN)
        pc += 4;
      else
        pc += 2;
    break;

    case 0x5000:        // (5XY0) Skips next instruction if VX == VY
      if (VX == VY)
        pc += 4;
      else
        pc += 2;
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
      switch (opcode & 0x000F)
      {
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
          VF = (VX >= VY) ? 1 : 0;
          VX -= VY;
          pc += 2;
        break;

        case 0x0006:    // (8XY6) Right shift VX
          VF = VX & 0x0001;
          VX >>= 1;
          pc += 2;
        break;

        case 0x0007:    // (8XY7) VX = VY - VX
          VF = (VY >= VX) ? 1 : 0;
          VX = VY - VX;
          pc += 2;
        break;

        case 0x000E:    // (8XYE) Left shift VX
          if (VX & 0x80)
            VF = 1;
          else
            VF = 0;
          VX <<= 1;
          pc += 2;
        break;
      }
    break;

    case 0x9000:       // (9XY0) Skip next instruction if VX != VY
      if (VX != VY)
        pc += 4;
      else
        pc += 2;
    break;

    case 0xA000:      // (ANNN) Set index register to NNN
      I = NNN;
      pc += 2;
    break;

    case 0xB000:      // (BNNN) Jump to address NNN + V0
      pc = NNN + V[0];
    break;

    case 0xC000:      // (CXNN) Set VX to the bitwise and of a random number and NN
      VX = rand() & NN;
      pc += 2;
    break;

    case 0xD000:      // (DXYN) Draw a sprite at (VX,VY) with a width of 8 pixels and height of N pixels
    {
      unsigned short height = opcode & 0x000F;
      VF = 0;
      for (int row = 0; row < height; ++row)
      {
        unsigned short pixel = memory[I + row];
        for (int col = 0; col < 8; ++col)
        {
          if (pixel & (0x80 >> col))
            {
              int idx = ((VY + row) % 32) * 64 + ((VX + col) % 64);
              if (gfx[idx])
                VF = 1;
              gfx[idx] ^= 1;
            }
        }
      }
      drawFlag = true;
      pc += 2;
    }
    break;

    case 0xE000:
    switch(opcode & 0x000F)
    {
      case 0x000E:    // (EX9E) Skip next instruction if key[VX] is pressed
        if (key[VX])
          pc += 4;
        else
          pc += 2;
      break;

      case 0x0001:    // (EXA1) Skip next instruction if key[VX] is not pressed
        if (!(key[VX]))
          pc += 4;
        else
          pc += 2;
      break;
    }
    break;

    case 0xF000:
    switch(opcode & 0x00FF)
    {
      case 0x0007:    // (FX07) Set VX equal to value of delay timer
        VX = delay_timer;
        pc += 2;
      break;
      case 0x000A:    // (FX0A) A key press is waited, then stored in VX (all instructions halted until next key event) 
      {
        bool keyPressed = false;
        for (int i = 0; i < 16; ++i)
          if (key[i])
          {
            VX = i;
            keyPressed = true;
            break;
          }
        if (keyPressed)
          pc += 2;
      }
      break;
      case 0x0015:    // (FX15) Set delay timer to VX
        delay_timer = VX;
        pc += 2;
      break;
      case 0x0018:    // (FX18) Set sound timer to VX
        sound_timer = VX;
        pc += 2;
      break;
      case 0x001E:    // (FX1E) Add VX to I
        I += VX;
        pc += 2;
      break;
      case 0x0029:    // (FX29) Set I to the location of the sprite for the character in VX
        I = VX * 5;
        pc += 2;
      break;
      case 0x0033:    // (FX33) Store binary coded decimal representation of VX
        memory[I] = VX / 100;
        memory[I + 1] = (VX / 10) % 10;
        memory[I + 2] = VX % 10;
        pc += 2;
      break;
      case 0x0055:    // (FX55) Store V0 - VX in memory, starting at address I
        for (int i = 0; i <= ((opcode & 0x0F00) >>8); ++i)
          memory[I + i] = V[i];
        pc += 2;
      break;
      case 0x0065:    // (FX65) Load V0 - VX with values from memory, starting at address I
        for (int i = 0; i <= ((opcode & 0x0F00) >>8); ++i)
          V[i] = memory[I + i];
        pc += 2;
      break;
    }
    break;
  }
}

bool chip8::loadApplication(const char *file)
{
    initalize();

    FILE *fp = fopen(file, "rb");
    if (fp == nullptr)
    {
        std::printf("Error: could not open file '%s'\n", file);
        return false;
    }

    // Get file size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    // Check it fits in memory (memory from 0x200 to 0xFFF = 3584 bytes)
    if (size > (4096 - 512))
    {
        std::printf("Error: ROM too large (%ld bytes)\n", size);
        fclose(fp);
        return false;
    }

    // Load ROM into memory starting at 0x200
    if (fread(memory + 512, 1, size, fp) != (size_t)size)
    {
        std::printf("Error: could not read file '%s'\n", file);
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}
