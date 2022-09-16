#include "Chip8.h"
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>

#define MEMORY_SIZE 4096
#define ROM_START_ADDRESS 0x200
#define FONT_SET_SIZE 80
#define FONT_SIZE 5
#define FONT_SET_START_ADDRESS 0x50
#define REGISTERS 16
#define GFX_WIDTH 64
#define GFX_HEIGHT 32

uint8_t fontSet[FONT_SET_SIZE] = {
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

Chip8::Chip8() {
    pc = ROM_START_ADDRESS;
    memory = std::vector<uint8_t>(MEMORY_SIZE);
    V = std::vector<uint8_t>(REGISTERS);
    I = 0x00;
    DT = 0x00;
    ST = 0x00;

    lastTime = std::chrono::high_resolution_clock::now();

    // Load font set in memory
    for (uint8_t i = 0; i < FONT_SET_SIZE; ++i) {
        memory[FONT_SET_START_ADDRESS + i] = fontSet[i];
    }
}

void Chip8::stepTimers() {
    if (DT > 0) DT--;
}

void Chip8::loadProgram(const char *buffer, int size) {
    for (int i = 0; i < size; ++i) {
        const auto opcode = uint8_t (buffer[i]);
        memory[ROM_START_ADDRESS + i] = opcode;
    }
}

void Chip8::execute() {
    const uint8_t p1 = memory[pc];
    const uint8_t p2 = memory[pc + 1];
    const uint8_t prefix = (p1 & 0XF0) >> 4;
    uint16_t opcode =  (p1 << 8) | p2;

    printf("RAM [ 0x%04x ] opcode [ 0x%04x ]\n", pc, opcode);

    switch (prefix) {
        case 0x0: {
            // Clear the display
            if (opcode == 0X00E0) {
                clearDisplay();
                pc += 2;
//                printf("Clear display");
            }

            // Return from a subroutine
            if (opcode == 0x00EE) {
                pc = stack.top();
                stack.pop();
//                printf("Return from a subroutine");
            }

            break;
        }

        // Jump to address NNN
        case 0x1: {
            uint16_t add = (opcode & 0X0FFF);
            pc = add;
//            printf("Jump to address 0x%x", add);
            break;
        }

        // 2NNN - Execute subroutine starting at address NNN
        case 0x2: {
            uint16_t add = (opcode & 0X0FFF);
            stack.push(pc + 2);
            pc = add;
//            printf("Execute subroutine starting at address 0x%x", add);
            break;
        }

        // 3XNN - Skip the following instruction if the value of register VX equals NN
        case 0x3: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t val = opcode & 0x00FF;
            V[x] == val ? pc += 4 : pc += 2;
//            printf("Skip the following instruction if the value of register V%x equals 0x%x", r, val);
            break;
        }

        // 4XNN - Skip the following instruction if the value of register VX is not equal to NN
        case 0x4: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t val = (opcode & 0X00FF);
            V[x] != val ? pc += 4 : pc +=2;
//            printf("Skip the following instruction if the value of register V%x is not equal to 0x%x", r, val);
            break;
        }

        // 5XY0 - Skip the following instruction if the value of register VX is equal to the value of register VY
        case 0x5: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            V[x] == V[y] ? pc += 4 : pc += 2;
//            printf("Skip the following instruction if the value of register V%x is equal to the value of register V%x", rx, ry);
            break;
        }

        // 6XNN - Store number NN in register VX
        case 0x6: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t val = opcode & 0X00FF;
            V[x] = val;
            pc += 2;
//            printf("Store number 0x%x in register V%x", val, x, V[x]);
            break;
        }

        // 7XNN - Add the value NN to register VX
        case 0X7: {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t val = opcode & 0X00FF;
            V[x] += val;
            pc += 2;
//            printf("Add the value %x to register V%x", val, r);
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
//                    V[x] = V[y] << 1;
//                    pc += 2;

                    V[0xF] = (V[x] & 0x80u) >> 7u;
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
//            printf("Skip the following instruction if the value of register V%x is not equal to the value of register V%x", rx, ry);
            break;
        }

        // ANNN - Store memory address NNN in register I
        case 0xA: {
            uint16_t add = opcode & 0X0FFF;
            I = add;
            pc += 2;
//            printf("Store memory address 0x%x in register I", add);
            break;
        }

        // BNNN - Jump to address NNN + V0
        case 0xB: {
            uint16_t add = opcode & 0x0FFF;
            pc = add + V[0];
//            printf("Jump to address 0X%x + V0", add);
            break;
        }

        // CXNN - Set VX to a random number with a mask of NN
        case 0xC: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t mask = opcode & 0X00FF;
            uint8_t val = (std::rand() % 256) & mask;
            V[x] = val;
            pc += 2;
//            printf("Set V%x to a random number with a mask of 0x%x", rx, val);
            break;
        }

        // DXYN - Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
        // Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
        case 0xD: {
            uint8_t x = (opcode & 0x0f00) >> 8;
            uint8_t y = (opcode & 0x00f0) >> 4;
            uint8_t spriteHeight = opcode & 0x000f;
            V[0xf] = 0;

            for (int i = 0; i < spriteHeight; ++i) {
                uint8_t spriteByte = memory[I + i];

                for (int j = 0; j < 8; ++j) {
                    uint8_t spritePixel = spriteByte & (0x80 >> j);
                    uint32_t* screenPixel = &gfx[(V[y] + i) * GFX_WIDTH + (V[x] + j)];

//                     Sprite pixel is on
                    if (spritePixel) {
                        // Screen pixel also on - collision
                        if (*screenPixel == 0xffffffff) {
                            V[0xf] = 1;
                        }
//                         Effectively XOR with the sprite pixel
                        *screenPixel ^= 0xffffffff;
                    }
                }
            }

            pc += 2;
//            printf("Draw sprite");
            break;
        }

        case 0xE: {
            uint8_t x = (opcode & 0X0F00) >> 8;
            uint8_t suffix = opcode & 0X0FF;

            // EX9E - Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
            if (suffix == 0x9E) {
//                printf("***************\n");
                uint8_t key = V[x];
                keypad[key] == 1 ? pc += 4 : pc += 2;
            }

            // EXA1 - Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
            if (suffix == 0xA1) {
//                printf("EXA1 [0x%x] - V%x = 0x%x\n", opcode, x, V[x]);
                uint8_t key = V[x];
                keypad[key] == 0 ? pc += 4 : pc += 2;
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
//                    printf("Wait for a keypress and store the result in register V%x\n", x);

                    bool keyPressed = false;

                    for (int i = 0; i < 16; ++i) {
                        if (keypad[i] == 1) {
                            V[x] = uint8_t(i);
                            keyPressed = true;
                        }
                    }

                    if (keyPressed) {
                        pc += 2;
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
                    I = (fontAdd & 0x00ff);
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

                default: break;
            }

            break;
        }
        default: printf("Unknown prefix 0x%x", prefix);
        }

        if (DT > 0) DT--;

//        auto currentTime = std::chrono::high_resolution_clock::now();
//        auto difference = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
//
//        printf("Difference %lld \n", difference);
//
//        if (difference >= 1 / 60 ) {
//            lastTime = currentTime;
//
//            if (DT > 0) DT--;
//        }

//    printf("\n");
}

void Chip8::DebugRam() {
    for(int i = 0; i < memory.size(); ++i) {
        printf("0x%x | %d -> ", i, i);
        printf("%x\n", memory[i]);
    }
}

void Chip8::clearDisplay() {
    memset(gfx, 0, sizeof(gfx));
}
