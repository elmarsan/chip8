# Chip 8 Interpreter (Remake 2024)

This is a remake of my Chip 8 interpreter originally written in 2022. The original version can be found in the `v1_2022` directory.
## Build

### Windows

Requires MSVC and access to `cl.exe`

1. Run `build_windows.bat`

### Linux

Depends on X11 window system, Wayland is not supported.
Requires c++20 compiler (clang >= 17 and gcc >= 13)

1. Execute `build_linux.sh`

### Cmake

You can compile for X11, WIN or SDL platforms.

If you wish to use SDL, you must first get the submodule:
`git submodule update --init external/SDL`

1. Configure CMake for your target platform
`cmake -B build -DPLATFORM=<target_platform>`

2. Build the project:
`cmake --build build`
