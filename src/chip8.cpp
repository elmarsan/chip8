#include "chip8.h"
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <string>

Chip8::Chip8()
{
    // Load fontset
    std::memcpy(&memory[0x50], fontset, sizeof(fontset));
}

void Chip8::LoadRom(const std::string& path)
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

void Chip8::ExecuteNext()
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
        const auto x = (opcode & 0x0f00) >> 8;
        const auto y = (opcode & 0x00f0) >> 4;
        const auto spriteHeight = opcode & 0x000f;
        regs[0xf] = 0;

        for (int i = 0; i < spriteHeight; ++i)
        {
            const auto spriteByte = memory[ri + i];

            for (int j = 0; j < 8; ++j)
            {
                const auto sprite = spriteByte & (0x80 >> j);
                auto* pixel = &videoBuffer[(regs[y] + i) * chip8Width + (regs[x] + j)];

                if (sprite)
                {
                    // Screen pixel also on - collision
                    if (*pixel)
                    {
                        regs[15] = 1;
                    }

                    *pixel ^= 0x0000ff00;
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
