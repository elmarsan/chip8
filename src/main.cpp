#include "App.h"

#include <fstream>
#include <stdexcept>

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

    Keyboard keyboard{};
    Chip8 chip8(&keyboard);
    chip8.LoadRom(buffer, int(size));

    App app(chip8, &keyboard, 10, 64, 32, 15, rom);

    app.OnExecute();

    return 0;
}