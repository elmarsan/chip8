#include <cstdlib>
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <random>

#include "bit.h"
#include "chip8.h"
#include "input.h"

Chip8::Chip8()
{
    std::srand(time(NULL));

    // Load fontset
    std::memcpy(&memory[0x50], fontset, sizeof(fontset));
}

bool Chip8::LoadRom(const std::string& path)
{
    std::fstream fileStream{path, std::ios::binary | std::ios::in};
    if (!fileStream.is_open())
    {
        std::cout << std::format("Failed to open rom: {}\n", path.c_str());
        return false;
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

    return true;
}

void Chip8::ExecuteNext()
{
    const auto hi = memory[pc];
    const auto lo = memory[pc + 1];
    const auto opcode = U8_CONCAT(hi, lo);
    const auto prefix = (hi & 0xf0) >> 4;
    
    switch (prefix)
    {
    case 0:
    {
        const auto suffix = opcode & 0xf;

        switch (suffix)
        {
        // 0NNN
        case 0:
        {
            std::memset(videoBuffer, 0, sizeof(videoBuffer));
            pc += 2;
            break;
        }

        // 00EE
        case 0xe:
        {
            pc = stack.top();
            stack.pop();
            break;
        }
        }

        break;
    }
    // JUMP NNN
    case 1:
    {
        pc = (opcode & 0x0fff);
        break;
    }
    // CALL NNN
    case 2:
    {
        stack.push(pc + 2);
        pc = (opcode & 0x0fff);
        break;
    }
    // 3XNN
    case 3:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        const auto value = (opcode & 0x00ff);
        if (regs[x] == value)
        {
            pc += 4;
            break;
        }
        pc += 2;
        break;
    }
    // 4XNN
    case 4:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        if (regs[x] != lo)
        {
            pc += 4;
            break;
        }
        pc += 2;
        break;
    }
    // 5XY0
    case 5:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        const auto y = (opcode & 0x00f0) >> 4;
        if (regs[x] == regs[y])
        {
            pc += 4;
            break;
        }
        pc += 2;
        break;
    }
    // 6XNN
    case 6:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        regs[x] = lo;
        pc += 2;
        break;
    }
    // 7XNN
    case 7:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        regs[x] += lo;
        pc += 2;
        break;
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
            uint16_t sum = regs[x] + regs[y];
            regs[x] = sum & 0xFF;
            regs[15] = (sum > 0xFF);
            break;
        }
        // 8XY5
        case 5:
        {
            regs[15] = !(regs[y] > regs[x]);
            regs[x] -= regs[y];
            break;
        }
        // 8XY6
        case 6:
        {
            regs[x] = (regs[y] >> 1);
            regs[15] = READ_BIT(regs[y], 0);
            break;
        }
        // 8XY7
        case 7:
        {
            regs[x] = regs[y] - regs[x];
            regs[15] = regs[y] > regs[x];
            break;
        }
        // 8XYE
        case 0xe:
        {
            regs[x] = (regs[y] << 1);
            regs[15] = READ_BIT(regs[y], 7);
            break;
        }
        }

        pc += 2;
        break;
    }
    // 9XY0
    case 9:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        const auto y = (opcode & 0x00f0) >> 4;
        if (regs[x] != regs[y])
        {
            pc += 4;
            break;
        }
        pc += 2;
        break;
    }
    // ANNN
    case 0xa:
    {
        ri = (opcode & 0xfff);
        pc += 2;
        break;
    }
    // BNNN
    case 0xb:
    {
        pc = (opcode & 0x0fff) + regs[0];
        break;
    }
    // CXNN
    case 0xc:
    {
        const auto x = (opcode & 0x0f00) >> 8;
        regs[x] = (rand() % 0xFF) & (opcode & 0x00FF);
        pc += 2;
        break;
    }
    // DXYN
    case 0xd:
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
        break;
    }
    case 0xe:
    {
        const auto idx = (opcode & 0x0f00) >> 8;

        switch (lo)
        {
        // EX9E
        case 0x9e:
        {
            if (IsKeyPressed(regs[idx]))
            {
                pc += 4;
            }
            else 
            {
                pc += 2;
            }
            break;
        }
        // EXA1
        case 0xa1:
        {
            if (!IsKeyPressed(regs[idx]))
            {
                pc += 4;
            }
            else 
            {
                pc += 2;
            }
            break;
        }
        }

        break;
    }
    case 0xf:
    {
        const auto idx = (opcode & 0x0f00) >> 8;

        switch (lo)
        {
        // FX07
        case 0x07:
        {
            regs[idx] = rdelay;
            pc += 2;
            break;
        }
        // FX0A
        case 0x0a:
        {
            for (int i = 0; i < inputKeyCount; i++)
            {
                if (IsKeyPressed(i))
                {
                    regs[idx] = i;
                    break;
                }
            }
            // Block until key pressed
            pc -= 2;
            break;
        }
        // FX15
        case 0x15:
        {
            rdelay = regs[idx];
            pc += 2;
            break;
        }
        // FX18
        case 0x18:
        {
            rsound = regs[idx];
            pc += 2;
            break;
        }
        // FX1E
        case 0x1e:
        {
            ri += regs[idx];
            pc += 2;
            break;
        }
        // FX29
        case 0x29:
        {
            const auto spriteAdd = (spriteSize * regs[idx]) + spriteStartAddress;
            ri = (spriteAdd & 0x00FF);
            pc += 2;
            break;
        }
        // FX33
        case 0x33:
        {
            const auto x = regs[idx];
            memory[ri] = x / 100;
            memory[ri + 1] = (x % 100) / 10;
            memory[ri + 2] = ((x % 100) % 10) / 1;
            pc += 2;
            break;
        }
        // FX55
        case 0x55:
        {
            for (int i = 0; i <= idx; ++i)
            {
                memory[ri + i] = regs[i];
            }
            pc += 2;
            break;
        }
        // FX65
        case 0x65:
        {
            for (int i = 0; i <= idx; ++i)
            {
                regs[i] = memory[ri + i];
            }
            pc += 2;
            break;
        }
        }

        break;
    }

    default:
        std::cout << std::format("Unimplementede 0x{:04x}, prefix {}\n", opcode, prefix);
        abort();
    }

    if (rdelay > 0) 
    {
        rdelay--;
    }

    if (rsound > 0)
    {
        if(rsound == 1)
        {
            std::cout << "BEEP\n";
        }
        rsound--;
    }
}
