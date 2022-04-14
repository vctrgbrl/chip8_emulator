cmake_minimum_required(VERSION 3.10)

project(CHIP8 VERSION 1.0)

include_directories(include)

add_executable(CHIP8 
	src/Emulator/main.c
	src/Emulator/app.c
	src/CHIP8/chip8_instructions.c
	src/CHIP8/chip8_utils.c
	src/Debugger/debugger.c)

target_link_libraries(CHIP8 SDL2main SDL2 dl)