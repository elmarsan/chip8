//
// Created by Elias Martinez on 7/9/22.
//

#include <cstdint>
#include <vector>
#include <stack>
#include <chrono>

#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

class Chip8 {
private:
    uint16_t pc;
    std::vector<uint8_t> memory;

    // General purpose registers
    std::vector<uint8_t> V;

    // Delay timer
    uint8_t DT;

    // Sound timer
    uint8_t ST;

    // Holds memory address
    uint16_t I;

    std::stack<uint16_t> stack;

    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
public:
    Chip8();
    ~Chip8() = default;
    void loadProgram(const char *buffer, int size);
    void execute();
    void DebugRam();
    void stepTimers();
    void clearDisplay();
    uint32_t gfx[64 * 32]{};
    int keypad[16]{};
};

#endif //CHIP8_CHIP8_H
