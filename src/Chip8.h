#include "Keyboard.h"

#include <stack>

#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#define MEMORY_SIZE 4096
#define MEMORY_ROM_START_ADDRESS 0x200
#define REGISTERS 16

#define GFX_WIDTH 64
#define GFX_HEIGHT 32

#define FONT_SET_SIZE 80
#define FONT_SIZE 5
#define FONT_SET_START_ADDRESS 0x50

const uint8_t fontSet[FONT_SET_SIZE] = {
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

class Chip8 {
private:
    uint16_t pc;
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[REGISTERS];
    uint8_t DT{};
    uint8_t ST{};
    uint16_t I{};
    std::stack<uint16_t> stack;
    Keyboard *keyboard;
    uint32_t palette;
public:
    Chip8(Keyboard *k);

    ~Chip8() = default;

    void LoadRom(const char *buffer, int size);

    void ExecuteCycle();

    void ClearGfx();

    uint32_t gfx[GFX_WIDTH * GFX_HEIGHT]{};
};

#endif //CHIP8_CHIP8_H
