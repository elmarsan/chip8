#include "platform.h"
#include "input.h"

#include <format>
#include <iostream>

#include <SDL2/SDL.h>

SDL_Window* window;
SDL_Renderer* renderer;

int width;
int height;

// Input handling helper
int toggleKey(int scancode, bool pressed);

bool platform_create_window(const std::string& title, const int w, const int h)
{
    window = SDL_CreateWindow("Chip8 SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cout << std::format("Unable to create SDL window: {}\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == nullptr)
    {
        std::cout << std::format("Unable to create SDL renderer: {}\n", SDL_GetError());
        return false;
    }

    width = w;
    height = h;

    return true;
}

bool platform_update_window(const uint32_t (&videoBuffer)[chip8Width * chip8Height])
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            std::cout << "Closing SDL window...\n";
            return false;
        }
        case SDL_KEYDOWN:
        {
            const auto scancode = event.key.keysym.scancode;
            toggleKey(scancode, true);
            break;
        }
        case SDL_KEYUP:
        {
            const auto scancode = event.key.keysym.scancode;
            toggleKey(scancode, false);
            break;
        }
        }
    }

    const int xScale = width / chip8Width;
    const int yScale = height / chip8Height;

    for (int y = 0; y < chip8Height; y++)
    {
        for (int x = 0; x < chip8Width; x++)
        {
            const auto pixelColor = videoBuffer[y * chip8Width + x];
            SDL_Rect rc;
            rc.x = x * xScale;
            rc.y = y * yScale;
            rc.w = xScale;
            rc.h = yScale;

            const auto r = (pixelColor >> 16) & 0xff;
            const auto g = (pixelColor >> 8) & 0xff;
            const auto b = pixelColor & 0xff;

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderFillRect(renderer, &rc);
        }
    }

    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    return true;
}

void platform_close_window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

int toggleKey(int scancode, bool pressed)
{
    switch (scancode)
    {
    case SDL_SCANCODE_1:
    {
        ToggleKey(KEY_CODE_1, pressed);
        break;
    }
    case SDL_SCANCODE_2:
    {
        ToggleKey(KEY_CODE_2, pressed);
        break;
    }
    case SDL_SCANCODE_3:
    {
        ToggleKey(KEY_CODE_3, pressed);
        break;
    }
    case SDL_SCANCODE_4:
    {
        ToggleKey(KEY_CODE_4, pressed);
        break;
    }
    case SDL_SCANCODE_Q:
    {
        ToggleKey(KEY_CODE_Q, pressed);
        break;
    }
    case SDL_SCANCODE_W:
    {
        ToggleKey(KEY_CODE_W, pressed);
        break;
    }
    case SDL_SCANCODE_E:
    {
        ToggleKey(KEY_CODE_E, pressed);
        break;
    }
    case SDL_SCANCODE_R:
    {
        ToggleKey(KEY_CODE_R, pressed);
        break;
    }
    case SDL_SCANCODE_A:
    {
        ToggleKey(KEY_CODE_A, pressed);
        break;
    }
    case SDL_SCANCODE_S:
    {
        ToggleKey(KEY_CODE_S, pressed);
        break;
    }
    case SDL_SCANCODE_D:
    {
        ToggleKey(KEY_CODE_D, pressed);
        break;
    }
    case SDL_SCANCODE_F:
    {
        ToggleKey(KEY_CODE_F, pressed);
        break;
    }
    case SDL_SCANCODE_Z:
    {
        ToggleKey(KEY_CODE_Z, pressed);
        break;
    }
    case SDL_SCANCODE_X:
    {
        ToggleKey(KEY_CODE_X, pressed);
        break;
    }
    case SDL_SCANCODE_C:
    {
        ToggleKey(KEY_CODE_C, pressed);
        break;
    }
    case SDL_SCANCODE_V:
    {
        ToggleKey(KEY_CODE_V, pressed);
        break;
    }
    }

    return -1;
}
