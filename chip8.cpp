#include "chip8.h"

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

  // Update Timers
  if (delay_timer > 0)
    --delay_timer;
  
  if (sound_timer > 0)
    --sound_timer;
}

