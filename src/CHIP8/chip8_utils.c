#include "chip8.h"
#include <stdio.h>
#include <string.h>

void CHIP8_Init(CHIP8* chip8) {
	chip8->SP = STACK;
	chip8->PC = 0x200;
	chip8->I = 0;
	chip8->IR_0 = 0;
	chip8->IR_1 = 0;
	memset(chip8->RAM, 0, 4096);
	memset(chip8->f_buffer, 0, 64*32);

	u_int8_t font[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	};

	for (int i = 0; i < sizeof(font); i++) {
		chip8->RAM[FONT_BUFFER + i] = font[i];
	}

	for (int i = 0; i < 16; i++) {
		chip8->V[i] = 0;
		chip8->keys[i] = 0;
	}
}

void CHIP8_Disassemble(CHIP8* chip8, int pc) {
	
	char* opcode;

	u_int8_t nibble_0 = chip8->RAM[pc + 0] & 0xF0;
	u_int8_t nibble_1 = chip8->RAM[pc + 0] & 0x0F;
	u_int8_t nibble_2 = chip8->RAM[pc + 1] & 0xF0;
	u_int8_t nibble_3 = chip8->RAM[pc + 1] & 0x0F;

	printf("0x%04X	", pc);
	printf("%02X ", chip8->RAM[pc + 0]);
	printf("%02X	", chip8->RAM[pc + 1]);

	switch (nibble_0)
	{
	case 0x00:
		if (chip8->RAM[pc + 1] == 0xE0) {
			printf("CLS");
		} else if (chip8->RAM[pc + 1] == 0xEE) {
			printf("RET");
		}
		break;
	case 0x10:
		printf("GOTO	0x%03x", (nibble_1 << 8) + nibble_2 + nibble_3);
		break;
	case 0x20:
		printf("CALL	0x%03X", (nibble_1 << 8) + nibble_2 + nibble_3);
		break;
	case 0x30:
		printf("SKIP.EQ	V%1X, 0x%02X", nibble_1, nibble_2 + nibble_3);
		break;
	case 0x40:
		printf("SKIP.NQ	V%1X, 0x%02X", nibble_1, nibble_2 + nibble_3);
		break;
	case 0x50:
		printf("SKIP.EQ	V%1X, V%1X", nibble_1, nibble_2 >> 4);
		break;
	case 0x60:
		printf("MOV	V%1X, 0x%02X", nibble_1, nibble_2 + nibble_3);
		break;
	case 0x70:
		printf("ADD	V%1X, 0x%02X", nibble_1, nibble_2 + nibble_3);
		break;
	case 0x80:
		switch (nibble_3)
		{
		case 0x00: opcode = "MOV"; break;
		case 0x01: opcode = "OR"; break;
		case 0x02: opcode = "AND"; break;
		case 0x03: opcode = "XOR"; break;
		case 0x04: opcode = "ADD"; break;
		case 0x05: opcode = "SUB"; break;
		case 0x06: opcode = "BTR"; break;
		case 0x07: opcode = "SUBB"; break;
		case 0x0E: opcode = "BTL"; break;
		default: opcode = ""; break;
		}
		printf("%s	V%1X, V%1X", opcode, nibble_1, nibble_2 >> 4);
		break;
	case 0x90:
		printf("SKIP.NE	V%1X, V%1X", nibble_1, nibble_2 >> 4);
		break;
	case 0xA0:
		printf("MOV	I, 0x%03X", ( nibble_1 << 8) + nibble_2 + nibble_3);
		break;
	case 0xB0:
		printf("JMP V0 + 0x%03X", ( nibble_1 << 8) + nibble_2 + nibble_3);
		break;
	case 0xC0:
		printf("MOV V%1X, RNG & 0x%02X", nibble_1, nibble_2 + nibble_3);
		break;
	case 0xD0:
		printf("DRAW	V%1X, V%1X, H%1X", nibble_1, nibble_2, nibble_3);
		break;
	case 0xE0:
		if (nibble_2 == 0x90 && nibble_3 == 0x0E) {
			printf("SKIP.K	V%1X", nibble_1);
		} else if (nibble_2 == 0xA0 && nibble_3 == 0x01) {
			printf("SKIP.NK	V%1X", nibble_1);
		}
		break;
	case 0xF0:
		if (nibble_2 + nibble_3 == 0x07) {
			printf("MOV	V%1X, DLY", nibble_1);
		} else if (nibble_2 + nibble_3 == 0x0A) {
			printf("WAIT	KEY 0x%1X", nibble_1);
		} else if (nibble_2 + nibble_3 == 0x15) {
			printf("MOV	DLY, V%1X", nibble_1);
		} else if (nibble_2 + nibble_3 == 0x18) {
			printf("MOV	SND, V%1X", nibble_1);
		} else if (nibble_2 + nibble_3 == 0x1E) {
			printf("ADD	I, V%1X", nibble_1);
		} else if (nibble_2 + nibble_3 == 0x29) {
			printf("MOV	I, CHAR[V%1x]", nibble_1);
		} else if (nibble_2 + nibble_3 == 0x33) {
			printf("STR	I, BCD[%1X]", nibble_1);
		}
		
		break;
	default:
		break;
	}

	printf("\n");
}

void CHIP8_LoadFile(CHIP8* chip8, char* filePath) {
	int file_size = 0;
	
	FILE* file = fopen(filePath, "rb");
	if (file == NULL) {
		printf("ERROR opening file %s\n", filePath);
		exit(-1);
		return;
	}

	fseek(file, 0L, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	fread(chip8->RAM + 0x200, 1, file_size, file);

	fclose(file);
}