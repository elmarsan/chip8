#include "Keyboard.h"

void Keyboard::KeyUp(int i) {
    keys[i] = 0;
}

void Keyboard::KeyDown(int i) {
    keys[i] = 1;
}

bool Keyboard::IsDown(int i) {
    return keys[i] == 1;
}

bool Keyboard::IsUp(int i) {
    return keys[i] == 0;
}
