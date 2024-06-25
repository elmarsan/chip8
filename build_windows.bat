@echo off

set MSV_INCLUDE_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.40.33807\include"
set MSV_LIB_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.40.33807\include"

clang++ ^
    -std=c++20 ^
    -I %MSV_INCLUDE_PATH% ^
    -L %MSV_LIB_PATH% ^
    .\src\main.cpp .\src\chip8.cpp .\platform_win32.cpp ^
    -o .\chip8.exe
