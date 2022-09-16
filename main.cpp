#include "Chip8.h"
#include "Display.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <map>

#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        throw std::runtime_error("Missing rom file argument");
    }

    char *rom = argv[1];
    std::ifstream file(rom, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();

    if (size < 0) {
        throw std::runtime_error("Rom file not found");
    }

    char *buffer = new char[size];

    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    Chip8 chip8;
    chip8.loadProgram(buffer, int(size));

    Display display(64, 32, 10, rom);

    SDL_Event event;

//    chip8.DebugRam();
    bool execute = true;

//    constexpr int FPS = 60;
//    constexpr int TICKS_PER_FRAME = 1000 / FPS;
//    uint64_t delta;

    while (execute) {
//        uint64_t startTime = SDL_GetTicks64();

        for (int i = 0; i < 10; ++i) {
            chip8.execute();
        }

        display.Draw(chip8.gfx);

        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_QUIT: {
                SDL_DestroyWindow(display.getWindow());
                SDL_Quit();
                execute = false;
                break;
            }

            case SDL_KEYUP: {
                int scancode = event.key.keysym.scancode;

                switch (scancode) {
                    case SDL_SCANCODE_0: { chip8.keypad[0] = 0; break; }
                    case SDL_SCANCODE_1: { chip8.keypad[1] = 0; break; }
                    case SDL_SCANCODE_2: { chip8.keypad[2] = 0; break; }
                    case SDL_SCANCODE_3: { chip8.keypad[3] = 0; break; }
                    case SDL_SCANCODE_4: { chip8.keypad[4] = 0; break; }
                    case SDL_SCANCODE_5: { chip8.keypad[5] = 0; break; }
                    case SDL_SCANCODE_6: { chip8.keypad[6] = 0; break; }
                    case SDL_SCANCODE_7: { chip8.keypad[7] = 0; break; }
                    case SDL_SCANCODE_8: { chip8.keypad[8] = 0; break; }
                    case SDL_SCANCODE_9: { chip8.keypad[9] = 0; break; }
                    case SDL_SCANCODE_A: { chip8.keypad[10] = 0; break; }
                    case SDL_SCANCODE_B: { chip8.keypad[11] = 0; break; }
                    case SDL_SCANCODE_C: { chip8.keypad[12] = 0; break; }
                    case SDL_SCANCODE_D: { chip8.keypad[13] = 0; break; }
                    case SDL_SCANCODE_E: { chip8.keypad[14] = 0; break; }
                    case SDL_SCANCODE_F: { chip8.keypad[15] = 0; break; }
                    default: break;
                }

                break;
            }

            case SDL_KEYDOWN: {
                int scancode = event.key.keysym.scancode;

                switch (scancode) {
                    case SDL_SCANCODE_0: { chip8.keypad[0] = 1; break; }
                    case SDL_SCANCODE_1: { chip8.keypad[1] = 1; break; }
                    case SDL_SCANCODE_2: { chip8.keypad[2] = 1; break; }
                    case SDL_SCANCODE_3: { chip8.keypad[3] = 1; break; }
                    case SDL_SCANCODE_4: { chip8.keypad[4] = 1; break; }
                    case SDL_SCANCODE_5: { chip8.keypad[5] = 1; break; }
                    case SDL_SCANCODE_6: { chip8.keypad[6] = 1; break; }
                    case SDL_SCANCODE_7: { chip8.keypad[7] = 1; break; }
                    case SDL_SCANCODE_8: { chip8.keypad[8] = 1; break; }
                    case SDL_SCANCODE_9: { chip8.keypad[9] = 1; break; }
                    case SDL_SCANCODE_A: { chip8.keypad[10] = 1; break; }
                    case SDL_SCANCODE_B: { chip8.keypad[11] = 1; break; }
                    case SDL_SCANCODE_C: { chip8.keypad[12] = 1; break; }
                    case SDL_SCANCODE_D: { chip8.keypad[13] = 1; break; }
                    case SDL_SCANCODE_E: { chip8.keypad[14] = 1; break; }
                    case SDL_SCANCODE_F: { chip8.keypad[15] = 1; break; }
                    default: break;
                }

                break;
            }
        }

//        delta = SDL_GetTicks64() - startTime;
//        printf("TICKS_PER_FRAME: %d delta: %llu\n", TICKS_PER_FRAME, delta);
//
//        if (TICKS_PER_FRAME > delta) {
//            printf("*********\n");
//
//            chip8.stepTimers();
//            SDL_Delay(TICKS_PER_FRAME - delta);
//        }
    }

    return 0;
}
