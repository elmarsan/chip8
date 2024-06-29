#include "input.h"

#include <array>
#include <iostream>
#include <format>

std::array<bool, KEY_CODE_COUNT> inputKeys;

void ToggleKey(int code, bool pressed)
{
    if (code < 0 || code > KEY_CODE_COUNT)
    {
        std::cout << std::format("Unknown key code {}\n", code);
        return;
    }

    inputKeys[code] = pressed;
}

[[nodiscard]] bool IsKeyPressed(int code) { return inputKeys[code]; }
