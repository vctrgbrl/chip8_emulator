#include <stdlib.h>

#define FRAME_BUFFER_SIZE 0x100
#define FONT_BUFFER 0x000
#define STACK 0xEA0
#define RAND_MAX 0x100

/*
*	Representation of the CHIP8 Machine
*/
typedef struct {
	u_int8_t RAM[4096];
	u_int8_t V[16];
	u_int16_t I;
	u_int16_t SP;
	u_int16_t PC;
	u_int8_t IR_0;
	u_int8_t IR_1;
	u_int8_t delay;
	u_int8_t sound;
	u_int8_t has_drawn;
	u_int8_t keys[16];
	u_int8_t f_buffer[64][32];
} CHIP8;

/*
* CPU Initialization
*/
void CHIP8_Init(CHIP8* chip8);

/*
*	Disassembles the next instruction
*/
void CHIP8_Disassemble(CHIP8* chip8, int pc);

/*
*	Loads file content into RAM[0x200]
*/
void CHIP8_LoadFile(CHIP8* chip8, char* filePath);

/*
*	Execution of a single instruction, a cycle
*/
void CHIP8_Exec(CHIP8* chip8);