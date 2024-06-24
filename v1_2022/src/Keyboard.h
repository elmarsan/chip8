#ifndef CHIP8_KEYBOARD_H
#define CHIP8_KEYBOARD_H

#define TOTAL_KEYS 16

class Keyboard {
private:
    int keys[TOTAL_KEYS];
public:
    ~Keyboard() = default;

    void KeyUp(int i);

    void KeyDown(int i);

    bool IsDown(int i);

    bool IsUp(int i);
};


#endif //CHIP8_KEYBOARD_H
