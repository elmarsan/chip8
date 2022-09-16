#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include <SDL2/SDL.h>
#include <cstdint>

class Display {
public:
    Display(const int &width, const int &height, const int &scale, char *title);
    void Draw(uint32_t *pixels);
    SDL_Window *getWindow();
private:
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    int pitch;
};


#endif //CHIP8_DISPLAY_H
