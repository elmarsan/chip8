/* #include <windows.h> */

#include "platform.h"

bool CreateWindow(const std::string& title, const int width, const int height) { return false; }

bool UpdateWindow(const uint32_t (&videoBuffer)[chip8Width * chip8Height]) { return false; }

void CloseWindow();
