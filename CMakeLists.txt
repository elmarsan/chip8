cmake_minimum_required(VERSION 3.22)

project(
    chip8 
    VERSION 0.2.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -fsanitize=address")

if (UNIX)
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})

    add_executable("${PROJECT_NAME}_x11" src/main.cpp src/chip8.cpp src/platform_x11.cpp)
    target_link_libraries("${PROJECT_NAME}_x11" ${X11_LIBRARIES})
endif (UNIX)

if (WIN32)
    add_executable(${PROJECT_NAME} src/main.cpp src/chip8.cpp src/platform_win32.cpp)
endif (WIN32)
