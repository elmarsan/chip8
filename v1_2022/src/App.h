#ifndef CHIP8_APP_H
#define CHIP8_APP_H

#include <SDL.h>
#include "Keyboard.h"
#include "Chip8.h"

class App {
private:
    bool running;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    Keyboard *keyboard;
    Chip8 chip8;
    int num_cycles;

    int width;
    int height;
    int scale;
    char *title;

    bool OnInit();

    void OnEvent(SDL_Event *Event);

    void OnLoop();

    void OnRender(uint32_t *pixels);

    void OnCleanup();

public:
    App(
            Chip8 &chip8,
            Keyboard *keyboard,
            int num_cycles,
            int width,
            int height,
            int scale,
            char *title
    );

    void OnExecute();
};


#endif //CHIP8_APP_H
