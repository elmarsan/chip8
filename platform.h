#pragma once

#include <cstdint>
#include <string>

extern uint32_t videoBuff[64 * 32]; 

bool CreateWindow(const std::string& title, const int width, const int height);
bool UpdateWindow();
void CloseWindow();
