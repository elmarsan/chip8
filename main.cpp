#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <stack>
#include <cstring>
#include <cassert>

#include "platform.h"

#define U8_CONCAT(b0, b1) ((static_cast<uint8_t>(b0) << 8) | static_cast<uint8_t>(b1))

#define READ_BIT(byte, num) (((byte) >> (num)) & 1)

constexpr int screenWidth = 64;
constexpr int screenHeight = 32;

constexpr uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

struct Chip8
{
    // General purpose registers
    // reg[15] = flag register
    uint8_t regs[16];

    // Special registers
    uint16_t pc;
    uint16_t ri;
    uint8_t rdelay;
    uint8_t rsound;

    uint8_t memory[4096]{};
    uint32_t videoBuffer[screenWidth * screenHeight]{};

    std::stack<uint16_t> stack;

    Chip8()
    {
        // Load fontset
        std::memcpy(&memory[0x50], fontset, sizeof(fontset));
    }

    void LoadRom(const std::string& path)
    {
        std::fstream fileStream{path, std::ios::binary | std::ios::in};
        if (!fileStream.is_open())
        {
            std::cout << std::format("Failed to open rom: {}\n", path.c_str());
        }

        pc = 512;
        char c[2];
        int memoryOffset = 512;
        while (fileStream.readsome(c, 2))
        {
            memory[memoryOffset] = c[0];
            memory[memoryOffset + 1] = c[1];
            memoryOffset += 2;
        }
    }

    void ExecuteNext()
    {
        const auto hi = memory[pc];
        const auto lo = memory[pc + 1];
        const auto opcode = U8_CONCAT(hi, lo);
        const auto prefix = (hi & 0xf0) >> 4;

        std::cout << std::format("Executing 0x{:04x}, prefix {}\n", opcode, prefix);

        switch (prefix)
        {
        // JUMP NNN
        case 1:
        {
            pc = (opcode & 0xfff);
            return;
        }
        // CALL NNN
        case 2:
        {
            stack.push(pc + 2);
            pc = (opcode & 0xfff);
            return;
        }
        // rx = NN
        case 6:
        {
            const auto idx = (hi & 0xf) >> 4;
            regs[idx] = lo;
            pc += 2;
            return;
        }
        // rx += NN
        case 7:
        {
            const auto idx = (hi & 0xf) >> 4;
            regs[idx] += lo;
            pc += 2;
            return;
        }
        // ri = NNN
        case 0xA:
        {
            ri = (opcode & 0xfff);
            pc += 2;
            return;
        }
        // Draw sprite vxvy N
        case 0xD:
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            uint8_t spriteHeight = opcode & 0x000F;
            regs[0xf] = 0;

            for (int i = 0; i < spriteHeight; ++i)
            {
                uint8_t spriteByte = memory[ri + i];

                for (int j = 0; j < 8; ++j)
                {
                    uint8_t sprite = spriteByte & (0x80 >> j);
                    uint32_t* pixel = &videoBuffer[(regs[y] + i) * screenWidth + (regs[x] + j)];

                    if (sprite)
                    {
                        // Screen pixel also on - collision
                        if (*pixel)
                        {
                            regs[15] = 1;
                        }

                        *pixel ^= 0x00FF0000;
                    }
                }
            }

            pc += 2;
            return;
        }

        switch (opcode)
        {
        case 0x00e0:
            std::memset(videoBuffer, 0, sizeof(videoBuffer));
            pc += 2;
            break;
        default:
            std::cout << std::format("UNIMPLEMENTED 0x{:04x}\n", opcode);
            abort();
        }
    }
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Missing rom file\n";
        return 1;
    }

    Chip8 chip8;
    chip8.LoadRom(argv[1]);

    CreateWindow("Chip8", 64, 32);

    while(true)
    {
        UpdateWindow();

        chip8.ExecuteNext();
        std::memmove(videoBuff, chip8.videoBuffer, sizeof(chip8.videoBuffer));
    }
}
