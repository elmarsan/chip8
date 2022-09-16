#include "Display.h"

#include <SDL2/SDL.h>
#include <cstdint>

Display::Display(const int &width, const int &height, const int &scale, char *title) {
    pitch = sizeof(uint32_t) * width;

    window = SDL_CreateWindow(
        title,
        0,
        0,
        width * scale,
        height * scale,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
    );

    SDL_Init(SDL_INIT_VIDEO);
}

void Display::Draw(uint32_t *pixels) {
    SDL_UpdateTexture(texture, nullptr, pixels, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

SDL_Window *Display::getWindow() {
    return window;
}
