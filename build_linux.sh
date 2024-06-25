#!/bin/bash

clang++ \
    -std=c++20 \
    -stdlib=libstdc++ \
    -I/usr/include/c++/13 \
    -I/usr/lib/gcc/x86_64-linux-gnu \
    $(pkg-config --cflags x11) \
    ./src/main.cpp ./src/chip8.cpp ./src/platform_x11.cpp \
    -o chip8.exe \
    $(pkg-config --libs x11)
