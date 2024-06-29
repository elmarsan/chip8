#pragma once

enum KEY_CODE 
{
    KEY_CODE_1,
    KEY_CODE_2,
    KEY_CODE_3,
    KEY_CODE_4,
    KEY_CODE_Q,
    KEY_CODE_W,
    KEY_CODE_E,
    KEY_CODE_R,
    KEY_CODE_A,
    KEY_CODE_S,
    KEY_CODE_D,
    KEY_CODE_F,
    KEY_CODE_Z,
    KEY_CODE_X,
    KEY_CODE_C,
    KEY_CODE_V,

    KEY_CODE_COUNT,
};

void ToggleKey(int code, bool pressed); 

[[nodiscard]] bool IsKeyPressed(int code);
