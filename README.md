# Chip 8 Interpreter

## Build

### Windows

Requires clang >= 17 and msv

1. Open build.bat and modify `MSV_INCLUDE_PATH` `MSV_LIB_PATH` variables
2. Execute `build_windows.bat`

### Linux

Requires clang >= 17 and gcc >= 13

1. Execute `build_linux.sh`

### Cmake

1. `cmake -B build`
2. `cmake --build build`
