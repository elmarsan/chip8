#pragma once

#include <cstdint>
#include <string>
#include "chip8.h"

[[nodiscard]] bool CreateWindow(const std::string& title, const int width, const int height);
[[nodiscard]] bool UpdateWindow(const uint32_t (&buffer)[chip8Width * chip8Height]);
void CloseWindow();
