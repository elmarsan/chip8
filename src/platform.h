#pragma once

#include <cstdint>
#include <string>
#include "chip8.h"

[[nodiscard]] bool platform_create_window(const std::string& title, const int width, const int height);
[[nodiscard]] bool platform_update_window(const uint32_t (&buffer)[chip8Width * chip8Height]);
void platform_close_window();
