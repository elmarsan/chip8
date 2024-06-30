#include <chrono>
#include <iostream>
#include <thread>

#include "chip8.h"
#include "platform.h"

constexpr auto execPerTick = 12;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Missing rom file argument\n";
        return 1;
    }

    Chip8 chip8;
    if (!chip8.LoadRom(argv[1]))
    {
        return 1;
    }

    if (!platform_create_window("Chip8", 800, 600))
    {
        return 1;
    }

    const auto fps = 60;
    const auto frameDelay = 1000 / fps;

    while (true)
    {
        auto frameStart = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < execPerTick; i++)
        {
            chip8.ExecuteNext();
        }

        if (!platform_update_window(chip8.videoBuffer))
        {
            platform_close_window();
            return 0;
        }

        std::chrono::duration<float, std::milli> frameDuration = std::chrono::high_resolution_clock::now() - frameStart;
        auto frameTime = frameDuration.count();

        if (frameDelay > frameTime)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(frameDelay - frameTime)));
        }
    }
}
