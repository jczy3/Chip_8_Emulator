class chip8
{
public:
  chip8();
  ~chip8();
  unsigned char key[16];      // Store current state of the key
  unsigned char gfx[64 * 32]; // Graphics array (0 or 1)
  bool drawFlag;

  void emulateCycle();
  bool loadApplication(const char *file);

private:
  unsigned short pc;       // Program counter
  unsigned short opcode{}; // Current opcode
  unsigned short I{};      // Index register
  unsigned short sp;       // Stack pointer

  unsigned char V[16];        // V registers (V0-VF)
  unsigned short stack[16];   // Stack (16 levels)
  unsigned char memory[4096]; // Memory (4K)

  unsigned char delay_timer{};
  unsigned char sound_timer{};

  unsigned char chip8_fontset[80];

  void initalize();
};