#include "Chip8.h"
#include "Logger.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

const uint32_t colors [4]{
    0xFF000000, // red
    0x00FF0000, // green
    0x0000FF00, // blue
    0xFFBF0000, // orange
};

Chip8::Chip8(Keyboard *k) : keyboard(k) {
    std::srand(time(nullptr));

    pc = MEMORY_ROM_START_ADDRESS;
    palette = colors[std::rand() % 4];

    // Load font set in memory
    for (uint8_t i = 0; i < FONT_SET_SIZE; ++i) {
        memory[FONT_SET_START_ADDRESS + i] = fontSet[i];
    }
}

void Chip8::LoadRom(const char *buffer, int size) {
    for (int i = 0; i < size; ++i) {
        const auto opcode = uint8_t(buffer[i]);
        memory[MEMORY_ROM_START_ADDRESS + i] = opcode;
    }
}

void Chip8::ExecuteCycle() {
    const uint8_t p1 = memory[pc];
    const uint8_t p2 = memory[pc + 1];
    const uint8_t prefix = (p1 & 0XF0) >> 4;
    uint16_t opcode = (p1 << 8) | p2;

    LDEBUG("RAM[0x%04x] 0x%04x \n", pc, opcode);

    switch (prefix) {
        case 0x0: {
            // Clear the display
            if (opcode == 0X00E0) {
                ClearGfx();
                pc += 2;
            }

            // Return from subroutine
            if (opcode == 0x00EE) {
                pc = stack.top();
                stack.pop();
            }

            break;
        }

        // Jump to address NNN
        case 0x1: {
            uint16_t add = (opcode & 0X0FFF);
            pc = add;
            break;
        }

        // 2NNN - Execute subroutine starting at address NNN
        case 0x2: {
            uint16_t add = (opcode & 0X0FFF);
            stack.push(pc + 2);
            pc = add;
            break;
        }

        // 3XNN - Skip the following instruction if the value of register VX equals NN
        case 0x3: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t val = opcode & 0x00FF;
            V[x] == val ? pc += 4 : pc += 2;
            break;
        }

        // 4XNN - Skip the following instruction if the value of register VX is not equal to NN
        case 0x4: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t val = (opcode & 0X00FF);
            V[x] != val ? pc += 4 : pc += 2;
            break;
        }

        // 5XY0 - Skip the following instruction if the value of register VX is equal to the value of register VY
        case 0x5: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            V[x] == V[y] ? pc += 4 : pc += 2;
            break;
        }

        // 6XNN - Store number NN in register VX
        case 0x6: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t val = opcode & 0X00FF;
            V[x] = val;
            pc += 2;
            break;
        }

        // 7XNN - Add the value NN to register VX
        case 0X7: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t val = opcode & 0X00FF;
            V[x] += val;
            pc += 2;
            break;
        }

        case 0X8: {
            uint8_t suffix = opcode & 0x000F;

            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0X00F0) >> 4;

            switch (suffix) {
                // 8XY0 - Store the value of register VY in register VX
                case 0x0: {
                    V[x] = V[y];
                    pc += 2;
                    break;
                }

                // 8XY1 - Set VX to VX OR VY
                case 0x1: {
                    V[x] |= V[y];
                    pc += 2;
                    break;
                }

                // 8XY2 - Set VX to VX AND VY
                case 0x2: {
                    V[x] &= V[y];
                    pc += 2;
                    break;
                }

                // 8XY3 - Set VX to VX XOR VY
                case 0x3: {
                    V[x] ^= V[y];
                    pc += 2;
                    break;
                }

                // 8XY4 - Add the value of register VY to register VX
                // Set VF to 01 if a carry occurs
                // Set VF to 00 if a carry does not occur
                case 0x4: {
                    bool overflow = int(V[x] + V[y]) > 255;
                    V[x] += V[y];
                    overflow ? V[0xF] = 1 : V[0xF] = 0;
                    pc += 2;
                    break;
                }

                // 8XY5 - Subtract the value of register VY from register VX
                // Set VF to 00 if a borrow occurs
                // Set VF to 01 if a borrow does not occur
                case 0x5: {
                    bool borrow = V[x] > V[y];
                    borrow ? V[0xF] = 1 : V[0xF] = 0;
                    V[x] -= V[y];
                    pc += 2;
                    break;
                }

                // 8XY6 - Set register VF to the least significant bit prior to the shift
                case 0x6: {
                    V[0xF] = (V[x] & 0x1);
                    V[x] >>= 1;
                    pc += 2;
                    break;
                }

                // 8XY7 - Set register VX to the value of VY minus VX
                // Set VF to 00 if a borrow occurs
                // Set VF to 01 if a borrow does not occur
                case 0x7: {
                    bool borrow = V[y] > V[x];
                    borrow ? V[0xF] = 1 : V[0xF] = 0;
                    V[x] = V[y] - V[x];
                    pc += 2;
                    break;
                }

                // 8XYE - Store the value of register VY shifted left one bit in register VX¹
                // Set register VF to the most significant bit prior to the shift
                // VY is unchanged
                case 0xE: {
                    V[0xF] = (V[x] & 0x80) >> 7;
                    V[x] <<= 1;
                    pc += 2;
                    break;
                }
            }

            break;
        }

        // 9XY0 - Skip the following instruction if the value of register VX is not equal to the value of register VY
        case 0x9: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            V[x] != V[y] ? pc += 4 : pc += 2;
            break;
        }

        // ANNN - Store memory address NNN in register I
        case 0xA: {
            uint16_t add = opcode & 0X0FFF;
            I = add;
            pc += 2;
            break;
        }

        // BNNN - Jump to address NNN + V0
        case 0xB: {
            uint16_t add = opcode & 0x0FFF;
            pc = add + V[0];
            break;
        }

        // CXNN - Set VX to a random number with a mask of NN
        case 0xC: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t mask = opcode & 0X00FF;
            uint8_t val = (std::rand() % 256) & mask;
            V[x] = val;
            pc += 2;
            break;
        }

        // DXYN - Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
        // Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
        case 0xD: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            uint8_t spriteHeight = opcode & 0x000F;
            V[0xf] = 0;

            for (int i = 0; i < spriteHeight; ++i) {
                uint8_t spriteByte = memory[I + i];

                for (int j = 0; j < 8; ++j) {
                    uint8_t sprite = spriteByte & (0x80 >> j);
                    uint32_t *pixel = &gfx[(V[y] + i) * GFX_WIDTH + (V[x] + j)];

                    if (sprite) {
                        // Screen pixel also on - collision
                        if (*pixel) {
                            V[0xF] = 1;
                        }

                        *pixel ^= palette;
                    }
                }
            }

            pc += 2;
            break;
        }

        case 0xE: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t suffix = opcode & 0X0FF;

            // EX9E - Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
            if (suffix == 0x9E) {
                uint8_t key = V[x];
                keyboard->IsDown(key) ? pc += 4 : pc += 2;
            }

            // EXA1 - Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
            if (suffix == 0xA1) {
                uint8_t key = V[x];
                keyboard->IsUp(key) ? pc += 4 : pc += 2;
            }

            break;
        }

        case 0xF: {
            uint8_t suffix = opcode & 0x00FF;
            uint8_t x = (opcode & 0x0F00) >> 8;

            switch (suffix) {
                // FX07 - Store the current value of the delay timer in register VX
                case 0x07: {
                    V[x] = DT;
                    pc += 2;
                    break;
                }

                // FX0A - Wait for a keypress and store the result in register VX
                case 0x0A: {
                    for (int i = 0; i < TOTAL_KEYS; ++i) {
                        if (keyboard->IsDown(i)) {
                            V[x] = uint8_t(i);
                            pc += 2;
                            break;
                        }
                    }

                    break;
                }

                // FX15 - Set the delay timer to the value of register VX
                case 0x15: {
                    DT = V[x];
                    pc += 2;
                    break;
                }

                // FX18 - Set the sound timer to the value of register VX
                case 0x18: {
                    ST = V[x];
                    pc += 2;
                    break;
                }

                // FX1E - Add the value stored in register VX to register I
                case 0x1E: {
                    I += (V[x] & 0X00ff);
                    pc += 2;
                    break;
                }

                // FX29 - Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
                case 0x29: {
                    uint8_t fontAdd = (FONT_SIZE * V[x]) + FONT_SET_START_ADDRESS;
                    I = (fontAdd & 0x00FF);
                    pc += 2;
                    break;
                }

                // FX33 - Store the binary-coded decimal equivalent of the value stored in register VX at addresses I, I + 1, and I + 2
                case 0x33: {
                    memory[I] = V[x] / 100;
                    memory[I + 1] = (V[x] % 100) / 10;
                    memory[I + 2] = ((V[x] % 100) % 10) / 1;
                    pc += 2;
                    break;
                }

                // FX55 - Store the values of registers V0 to VX inclusive in memory starting at address I
                case 0X55: {
                    for (int i = 0; i <= x; ++i) {
                        memory[I + i] = V[i];
                    }

                    pc += 2;
                    break;
                }

                // FX65 - Fill registers V0 to VX inclusive with the values stored in memory starting at address I
                case 0x65: {
                    for (int i = 0; i <= x; ++i) {
                        V[i] = memory[I + i];
                    }

                    pc += 2;
                    break;
                }
            }

            break;
        }

        default:printf("Unknown prefix 0x%x", prefix);
    }

    if (DT > 0) DT--;
    if (ST > 0) ST--;
}


void Chip8::ClearGfx() {
    memset(gfx, 0, sizeof(gfx));
}
