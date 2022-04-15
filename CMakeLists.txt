cmake_minimum_required(VERSION 3.10)

project(CHIP8 VERSION 1.0)

include_directories(include)
include_directories(SDL/include)

set(BUILD_SHARED_LIBS off)

add_subdirectory(SDL)

add_executable(CHIP8 
	src/Emulator/main.c
	src/Emulator/app.c
	src/CHIP8/chip8_instructions.c
	src/CHIP8/chip8_utils.c)

target_link_libraries(CHIP8 PRIVATE SDL2-static)