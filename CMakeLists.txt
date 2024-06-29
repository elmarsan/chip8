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

if (NOT DEFINED PLATFORM)
    set(PLATFORM "SDL")
endif()

if(PLATFORM STREQUAL "SDL")
    include_directories(external/SDL)
    add_subdirectory(external/SDL)

    add_executable("${PROJECT_NAME}_sdl" src/main.cpp src/chip8.cpp src/platform_sdl.cpp src/input.cpp)
    target_link_libraries("${PROJECT_NAME}_sdl" PRIVATE SDL2)

elseif(PLATFORM STREQUAL "WIN")
    add_executable("${PROJECT_NAME}_win" src/main.cpp src/chip8.cpp src/platform_win32.cpp src/input.cpp)

elseif(PLATFORM STREQUAL "X11")
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})

    add_executable("${PROJECT_NAME}_x11" src/main.cpp src/chip8.cpp src/platform_x11.cpp src/input.cpp)
    target_link_libraries("${PROJECT_NAME}_x11" ${X11_LIBRARIES})

else()
    message(FATAL_ERROR "Unsupported platform: ${PLATFORM}. Please specify a valid platform.")
endif()
