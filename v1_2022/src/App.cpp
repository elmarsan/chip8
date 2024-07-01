#include "App.h"

#include <stdexcept>

App::App(
        Chip8 &chip8,
        Keyboard *keyboard,
        int num_cycles,
        int width,
        int height,
        int scale,
        char *title
) : chip8(chip8), keyboard(keyboard), num_cycles(num_cycles), width(width), height(height), scale(scale), title(title) {
    running = true;
    window = nullptr;
}

void App::OnExecute() {
    OnInit();

    SDL_Event Event;

    while (running) {
        while (SDL_PollEvent(&Event)) {
            OnEvent(&Event);
        }

        OnLoop();
        OnRender(chip8.gfx);
    }

    OnCleanup();
}

bool App::OnInit() {
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

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL could not be initialized");
    }

    return true;
}

void App::OnEvent(SDL_Event *Event) {
    switch (Event->type) {
        case SDL_QUIT: {
            OnCleanup();

            running = false;
            break;
        }

        case SDL_KEYUP: {
            int scancode = Event->key.keysym.scancode;

            switch (scancode) {
                case SDL_SCANCODE_0: { keyboard->KeyUp(0); break; }
                case SDL_SCANCODE_1: { keyboard->KeyUp(1); break; }
                case SDL_SCANCODE_2: { keyboard->KeyUp(2); break; }
                case SDL_SCANCODE_3: { keyboard->KeyUp(3); break; }
                case SDL_SCANCODE_4: { keyboard->KeyUp(4); break; }
                case SDL_SCANCODE_5: { keyboard->KeyUp(5); break; }
                case SDL_SCANCODE_6: { keyboard->KeyUp(6); break; }
                case SDL_SCANCODE_7: { keyboard->KeyUp(7); break; }
                case SDL_SCANCODE_8: { keyboard->KeyUp(8); break; }
                case SDL_SCANCODE_9: { keyboard->KeyUp(9); break; }
                case SDL_SCANCODE_A: { keyboard->KeyUp(10); break; }
                case SDL_SCANCODE_B: { keyboard->KeyUp(11); break; }
                case SDL_SCANCODE_C: { keyboard->KeyUp(12); break; }
                case SDL_SCANCODE_D: { keyboard->KeyUp(13); break; }
                case SDL_SCANCODE_E: { keyboard->KeyUp(14); break; }
                case SDL_SCANCODE_F: { keyboard->KeyUp(15); break; }
                default: break;
            }

            break;
        }

        case SDL_KEYDOWN: {
            int scancode = Event->key.keysym.scancode;

            switch (scancode) {
                case SDL_SCANCODE_0: { keyboard->KeyDown(0); break; }
                case SDL_SCANCODE_1: { keyboard->KeyDown(1); break; }
                case SDL_SCANCODE_2: { keyboard->KeyDown(2); break; }
                case SDL_SCANCODE_3: { keyboard->KeyDown(3); break; }
                case SDL_SCANCODE_4: { keyboard->KeyDown(4); break; }
                case SDL_SCANCODE_5: { keyboard->KeyDown(5); break; }
                case SDL_SCANCODE_6: { keyboard->KeyDown(6); break; }
                case SDL_SCANCODE_7: { keyboard->KeyDown(7); break; }
                case SDL_SCANCODE_8: { keyboard->KeyDown(8); break; }
                case SDL_SCANCODE_9: { keyboard->KeyDown(9); break; }
                case SDL_SCANCODE_A: { keyboard->KeyDown(10); break; }
                case SDL_SCANCODE_B: { keyboard->KeyDown(11); break; }
                case SDL_SCANCODE_C: { keyboard->KeyDown(12); break; }
                case SDL_SCANCODE_D: { keyboard->KeyDown(13); break; }
                case SDL_SCANCODE_E: { keyboard->KeyDown(14); break; }
                case SDL_SCANCODE_F: { keyboard->KeyDown(15); break; }
                default: break;
            }

            break;
        }
    }

}

void App::OnLoop() {
    for (int i = 0; i < num_cycles; ++i) {
        chip8.ExecuteCycle();
    }
}

void App::OnRender(uint32_t *pixels) {
    int pitch = sizeof(uint32_t) * width;

    SDL_UpdateTexture(texture, nullptr, pixels, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void App::OnCleanup() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_Quit();
}
