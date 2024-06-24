#!/bin/bash

clang++ \
    -std=c++20 \
    -stdlib=libstdc++ \
    -I/usr/include/c++/13 \
    -I/usr/lib/gcc/x86_64=linux-gnu \
    main.cpp \
    -o chip8.exe
