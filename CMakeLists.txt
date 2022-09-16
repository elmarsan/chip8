cmake_minimum_required(VERSION 3.23)
project(chip8)

SET(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOGGER")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(chip8 src/main.cpp src/Chip8.cpp src/App.cpp src/Keyboard.cpp src/Logger.h)
target_link_libraries(chip8 ${SDL2_LIBRARIES})
