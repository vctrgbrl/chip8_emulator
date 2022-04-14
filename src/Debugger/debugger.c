#include "debugger.h"
#include <stdio.h>
#include <string.h>

#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_YELLOW	"\x1b[33m"
#define ANSI_COLOR_BLUE		"\x1b[34m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_CYAN		"\x1b[36m"
#define ANSI_COLOR_RESET	"\x1b[0m"

int pow_int(int a, int b) {
	int p = 1;
	if (b < 0) return -1;
	
	for (int i = 0; i < b; i++) {
		p *= a;
	}
	return p;
}

int str_to_int(char* str) {
	int ret = 0;
	int mult = 1;
	int len = 0;
	int i = 0;
	char c = str[0];

	while (1) {
		c = str[len];
		if (c == '\0' || c == '\n') break;
		len++;
	}
	
	if (str[0] == '-') {
		i++;
		mult *= -1;
	}

	for (; i < len; i++) {
		c = str[i];
		if (c >= '0' && c <= '9') {
			ret += (c - '0') * pow_int(10, (len - i - 1));
		} else {
			return 0xFFFFFFFF;
		}
	}
	return ret * mult;
}

char** strsplit(char* str, char split, int* array_size) {
	
	int i = 0;
	int j = 0;
	int lastSplit = 0;
	*array_size = 1;
	char c = str[0];
	char** output;

	while (c != '\0') {
		if (c == '\n') {
			str[i-1] = '\0';
			break;
		}
		c = str[i++];
		
		if (c == split) {
			if (lastSplit) {
				continue;
			}
			lastSplit = 1;
			(*array_size)++;
		} else {
			lastSplit = 0;
		}
	}

	i = 0;
	lastSplit = 0;
	output = (char**)malloc(*array_size);
	output[j++] = str;
	c = str[0];

	while (c != '\0') {
		c = str[i];
		if (lastSplit && c != split) {
			output[j++] = (char*)(str + i);
			lastSplit = 0;
		}
		
		if (c == split) {
			str[i] = '\0';
			lastSplit = 1;
		}
		i++;
	}
	return output;
}

void DEBUG_print_registers(CHIP8* chip8) {
	printf("PC	=	%02X\n", chip8->PC);
	printf("I	=	%03X\n", chip8->I);
	printf("SP	=	%02X\n",chip8->SP);
	for (u_int8_t i = 0; i < 0xF; i++) {
		printf("V%1X	=	%02X\n", i, chip8->V[i]);
	}
	return;
}

void DEBUG_print_memory(CHIP8* chip8, u_int16_t address,u_int16_t n_bytes) {
	int j = 0;
	for (int i = 0; i < n_bytes; i++) {
		if (i%4 == 0) {
			printf("%04X	", address + i);
		}
		printf("%02X ", chip8->RAM[i + address]);
		if (i%4 == 3) {
			printf("\n");
		}
	}
}

void DEBUG_help() {
	printf("commands: \n");
	printf("	dis\n");
	printf("	s\n");
	printf("	print (addr : int) (n_bytes: int)\n");
	printf("	status\n");
}

void DEBUG_disassemble(CHIP8* chip8, int n) {
	for (int i = 0; i < n; i++) {
		CHIP8_Disassemble(chip8->RAM, chip8->PC + i*2);
	}
}

void DEBUG_step(CHIP8* chip8, int n) {
	for (int i = 0; i < n; i++) {
		CHIP8_Exec(chip8);
	}
}

void DEBUG_exec(CHIP8* chip8, char* instruction) {
	int arr_size = 0;
	char** parameters = strsplit(instruction, ' ', &arr_size);
	int params[2] = { 0,16 };

	for (int i = 1; i < arr_size; i++) {
		params[i-1] = str_to_int(parameters[i]);
	}

	if (strcmp(parameters[0], "help") == 0)			{ DEBUG_help(); }
	else if (strcmp(parameters[0], "exit") == 0)	{ exit(0); }
	else if (strcmp(parameters[0], "dis") == 0)	{ DEBUG_disassemble(chip8, params[0]); }
	else if (strcmp(parameters[0], "s") == 0)		{ DEBUG_step(chip8, params[0]); }
	else if (strcmp(parameters[0], "print") == 0)	{ DEBUG_print_memory(chip8, params[0], params[1]);}
	else if (strcmp(parameters[0], "status") == 0)	{ DEBUG_print_registers(chip8); }

	free(parameters);
}