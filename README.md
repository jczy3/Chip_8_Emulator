# Chip-8 Emulator

A fully functional CHIP-8 emulator written in modern C++ (C++20), implementing the complete virtual machine architecture.

This emulator can:

* Load and execute original CHIP-8 ROMs

* Run classic games like Pong, Tetris, and Space Invaders

* Render 64x32 monochrome graphics in real time

* Accurately emulate timers and sprite collision behavior

A ROM is a binary program file containing CHIP-8 instructions. The emulator loads the ROM into memory at address 0x200 and begins executing instructions through a fetch–decode–execute cycle.

This project focuses on low-level system design and emulation of the original CHIP-8 specification, including:

🧠 4KB memory architecture

🗂 16 general-purpose 8-bit registers (V0–VF)

🧮 16-bit index register (I)

⏱ Delay and sound timers

📍 Program counter and stack implementation

🎮 Opcode fetch–decode–execute cycle

🖼 64x32 monochrome graphics buffer

🎯 Sprite rendering with collision detection

⌨ Keypad input handling

🔁 ROM loading and execution

Pong running on CHIP8 Emulator:
<img width="1920" height="1041" alt="image" src="https://github.com/user-attachments/assets/a760486b-0253-4458-9ab2-c06d8074015b" />
