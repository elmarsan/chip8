@echo off

if not exist ".\build" (
    mkdir ".\build"
)
cd .\build

cl ^
    /Zi ^
    /std:c++20 ^
    /EHsc ^
    ..\src\chip8.cpp ^
    ..\src\input.cpp ^
    ..\src\platform_win32.cpp ^
    ..\src\main.cpp ^
    /link user32.lib gdi32.lib

cd ..
