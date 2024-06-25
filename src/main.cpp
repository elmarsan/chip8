#include <iostream>
#include <cstring>

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

    while(true)
    {
        chip8.ExecuteNext();
        UpdateWindow(chip8.videoBuffer);
    }
}
