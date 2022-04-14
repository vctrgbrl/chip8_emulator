#include "chip8.h"

void DEBUG_print_registers(CHIP8* chip8);
void DEBUG_print_memory(CHIP8* chip8, u_int16_t address, u_int16_t n_bytes);
void DEBUG_help();
void DEBUG_disassemble(CHIP8* chip8, int n);
void DEBUG_step(CHIP8* chip8, int n);
void DEBUG_exec(CHIP8* chip8, char* instruction);