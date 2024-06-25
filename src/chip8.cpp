#include "chip8.h"
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include "bit.h"

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
    // 3XNN
    case 3:
    {
        const auto idx = (opcode & 0x0f00) >> 8;
        const auto value = (opcode & 0xff);
        if (regs[idx] == value)
        {
            pc += 4;
            return;
        }
        pc += 2;
        return;
    }
    // 4XNN
    case 4:
    {
        const auto idx = (opcode & 0x0f00) >> 8;
        if (regs[idx] != lo)
        {
            pc += 4;
            return;
        }
        pc += 2;
        return;
    }
    // 5XY0
    case 5:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        const auto y = (opcode & 0x00f0) >> 4;
        if (regs[x] == regs[y])
        {
            pc += 4;
            return;
        }
        pc += 2;
        return;
    }
    // 6XNN
    case 6:
    {
        const auto idx = (opcode & 0x0f00) >> 8;
        regs[idx] = lo;
        pc += 2;
        return;
    }
    // 7XNN
    case 7:
    {
        const auto idx = (opcode & 0x0f00) >> 8;
        regs[idx] += lo;
        pc += 2;
        return;
    }
    case 8:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        const auto y = (opcode & 0x00f0) >> 4;
        const auto suffix = (opcode & 0xf);

        switch (suffix)
        {
        // 8XY0
        case 0:
        {
            regs[x] = regs[y];
            break;
        }
        // 8XY1
        case 1:
        {
            regs[x] |= regs[y];
            break;
        }
        // 8XY2
        case 2:
        {
            regs[x] &= regs[y];
            break;
        }
        // 8XY3
        case 3:
        {
            regs[x] ^= regs[y];
            break;
        }
        // 8XY4
        case 4:
        {
            // carry
            if (static_cast<uint16_t>(regs[x]) + static_cast<uint16_t>(regs[y]) > 0xff)
            {
                regs[15] = 1;
            }
            else
            {
                regs[15] = 0;
            }
            regs[x] += regs[y];
            break;
        }
        // 8XY5
        case 5:
        {
            // borrow
            if (regs[x] > regs[y])
            {
                regs[15] = 1;
            }
            else
            {
                regs[15] = 0;
            }
            regs[x] -= regs[y];
            break;
        }
        // 8XY6
        case 6:
        {
            regs[15] = READ_BIT(regs[x], 0);
            regs[x] = (regs[x] >> 1);
            break;
        }
        // 8XY7
        case 7:
        {
            // borrow
            if (regs[x] > regs[y])
            {
                regs[15] = 1;
            }
            else
            {
                regs[15] = 0;
            }
            regs[x] = regs[y] - regs[x];
            break;
        }
        // 8XYE
        case 0xe:
        {

            regs[15] = READ_BIT(regs[x], 7);
            regs[x] = (regs[x] << 1);
        }
        }

        pc += 2;
        return;
    }
    // 9XY0
    case 9:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        const auto y = (opcode & 0x00f0) >> 4;
        if (regs[x] != regs[y])
        {
            pc += 4;
            return;
        }
        pc += 2;
        return;
    }
    // ANNN
    case 0xA:
    {
        ri = (opcode & 0xfff);
        pc += 2;
        return;
    }
    // BNNN
    case 0xB:
    {
        pc = (opcode & 0x0fff) + regs[0];
        return;
    }
    // DXYN
    case 0xD:
    {
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

    case 0xF:
    {
        const auto idx = (opcode & 0x0f00) >> 8;

        switch (lo)
        {
        // FX07
        case 0x07:
        {
            regs[idx] = rdelay;
            break;
        }
        // FX0A
        case 0x0a:
        {
            for (int i = 0; i < spriteCount; i++)
            {
                if (input[i])
                {
                    regs[idx] = i;
                    break;
                }
            }
        }
        // FX15
        case 0x15:
        {
            rdelay = regs[idx];
            break;
        }
        // FX18
        case 0x18:
        {
            rsound = regs[idx];
            break;
        }
        // FX1E
        case 0x1e:
        {
            ri += regs[idx];
            break;
        }
        // FX29
        case 0x29:
        {
            const auto spriteAdd = (spriteSize * regs[idx]) + spriteStartAddress;
            ri = (spriteAdd & 0x00FF);
            break;
        }
        // FX33
        case 0x33:
        {
            const auto x = regs[idx];
            memory[ri] = x / 100;
            memory[ri + 1] = (x % 100) / 10;
            memory[ri + 2] = ((x % 100) % 10) / 1;
            break;
        }
        // FX55
        case 0x55:
        {
            for (int i = 0; i <= idx; ++i)
            {
                memory[ri + i] = regs[i];
            }
            break;
        }
        // FX65
        case 0x65:
        {
            for (int i = 0; i <= idx; ++i)
            {
                regs[i] = memory[ri + i];
            }
            break;
        }
        }

        pc += 2;
        return;
    }
    }

    switch (opcode)
    {
    case 0x00e0:
    {
        std::memset(videoBuffer, 0, sizeof(videoBuffer));
        pc += 2;
        break;
    }
    case 0x00ee:
    {
        pc = stack.top();
        stack.pop();
        break;
    }
    default:
        std::cout << std::format("UNIMPLEMENTED 0x{:04x}\n", opcode);
        abort();
    }
}
