#include <iostream>
#include <cstring>
#include <chrono>

#include "platform.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Missing rom file argument\n";
        return 1;
    }

    Chip8 chip8;
    chip8.LoadRom(argv[1]);

    CreateWindow("Chip8", 800, 600);

    auto lastFrame = std::chrono::high_resolution_clock::now();

    while (true)
    {
        auto currentFrame = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // If 1/60th of a second has passed
        if (deltaTime.count() >= 1.0f / 60.0f)
        {
            if (chip8.rdelay > 0)
            {
                --chip8.rdelay;
            }
            if (chip8.rsound > 0)
            {
                --chip8.rsound;
            }

            // Reset the last_time to current_time
            lastFrame = currentFrame;
        }

        chip8.ExecuteNext();
        UpdateWindow(chip8.videoBuffer);
    }
}
