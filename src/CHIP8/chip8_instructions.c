#include "chip8.h"
#include <stdio.h>
#include <string.h>

void CHIP8_ClearScreen(CHIP8* chip8) {
	memset((void*)(chip8->f_buffer), 0, 64*32);
	chip8->has_drawn = 1;
}

void CHIP8_Return(CHIP8* chip8) {
	chip8->PC = (chip8->RAM[chip8->SP] & 0x0F) << 8 | chip8->RAM[chip8->SP + 1];
	chip8->SP += 2;
}

void CHIP8_GoTo(CHIP8* chip8) {
	chip8->PC = (chip8->IR_0 & 0x0F) << 8 | chip8->IR_1;
	chip8->PC -= 2; // The CHIP8_Exec Function will add 2 at the end, we should correct it
}

void CHIP8_Call(CHIP8* chip8) {
	chip8->SP -= 2;
	chip8->RAM[chip8->SP] = (chip8->PC & 0xFF00) >> 8;
	chip8->RAM[chip8->SP + 1] = (chip8->PC & 0xFF);
	chip8->PC = ((chip8->IR_0 & 0x0F) << 8) | chip8->IR_1;
	chip8->PC -= 2;
}

void CHIP8_SkipOnEqConst(CHIP8* chip8) {
	if (chip8->V[chip8->IR_0 &0x0F] == chip8->IR_1) {
		chip8->PC += 2;
	}
}

void CHIP8_SkipOnNotEqConst(CHIP8* chip8) { 
	if (chip8->V[chip8->IR_0 &0x0F] != chip8->IR_1) {
		chip8->PC += 2;
	}
}

void CHIP8_SkipOnEqReg(CHIP8* chip8) {
	if (chip8->V[chip8->IR_0 &0x0F] == chip8->V[chip8->IR_1 >> 4]) {
		chip8->PC += 2;
	}
}

void CHIP8_SkipOnNotEqReg(CHIP8* chip8) {
	if (chip8->V[chip8->IR_0 &0x0F] != chip8->V[chip8->IR_1 >> 4]) {
		chip8->PC += 2;
	}
}

void CHIP8_MovConst(CHIP8* chip8) {
	chip8->V[chip8->IR_0 &0x0F] = chip8->IR_1;
}

void CHIP8_AddConst(CHIP8* chip8) {
	chip8->V[chip8->IR_0 &0x0F] += chip8->IR_1;
}

void CHIP8_Assign(CHIP8* chip8) {
	chip8->V[chip8->IR_0 &0x0F] = chip8->V[chip8->IR_1 >> 4];
}

void CHIP8_OR(CHIP8* chip8) {
	chip8->V[chip8->IR_0 &0x0F] |= chip8->V[chip8->IR_1 >> 4];
}

void CHIP8_AND(CHIP8* chip8) {
	chip8->V[chip8->IR_0 &0x0F] &= chip8->V[chip8->IR_1 >> 4];
}

void CHIP8_XOR(CHIP8* chip8) {
	chip8->V[chip8->IR_0 &0x0F] ^= chip8->V[chip8->IR_1 >> 4];
}

void CHIP8_ADD(CHIP8* chip8) {
	chip8->V[0xF] = 0;
	if (chip8->V[chip8->IR_0 &0x0F] > 0xFF - chip8->V[chip8->IR_1 >> 4]) {
		chip8->V[0xF] = 1;
	}
	
	chip8->V[chip8->IR_0 &0x0F] += chip8->V[chip8->IR_1 >> 4];
}

void CHIP8_SUB(CHIP8* chip8) {
	chip8->V[0xF] = 1;
	if (chip8->V[chip8->IR_0 &0x0F] < chip8->V[chip8->IR_1 >> 4]) {
		chip8->V[0xF] = 0;
	}

	chip8->V[chip8->IR_0 &0x0F] -= chip8->V[chip8->IR_1 >> 4];
}

void CHIP8_BITR(CHIP8* chip8) {
	chip8->V[0xF] = chip8->V[chip8->IR_0 & 0x0F] & 0x01;
	chip8->V[chip8->IR_0 & 0x0F] >>= 1;
}

void CHIP8_SUBB(CHIP8* chip8) {
	chip8->V[0xF] = 1;
	if (chip8->V[chip8->IR_0 &0x0F] > chip8->V[chip8->IR_1 >> 4]) {
		chip8->V[0xF] = 0;
	}
	chip8->V[chip8->IR_0 &0x0F] = chip8->V[chip8->IR_1 >> 4] - chip8->V[chip8->IR_0 &0x0F];
}

void CHIP8_BITL(CHIP8* chip8) {
	chip8->V[0xF] = chip8->V[chip8->IR_0 & 0x0F] >> 7;
	chip8->V[chip8->IR_0 & 0x0F] <<= 1;
}

void CHIP8_MovI(CHIP8* chip8) {
	chip8->I = (chip8->IR_0 & 0x0F) << 8 | chip8->IR_1;
}

void CHIP8_GoToRelV0(CHIP8* chip8) {
	chip8->PC = chip8->V[0] + (chip8->IR_0 & 0x0F) << 8 | chip8->IR_1;
}

void CHIP8_MovRand(CHIP8* chip8) {
	chip8->V[chip8->IR_0 & 0x0F] = rand() & chip8->IR_1;
}

void CHIP8_Draw(CHIP8* chip8) {
	u_int8_t x, y, h, pixel;
	x = chip8->V[chip8->IR_0 & 0x0F];
	y = chip8->V[(chip8->IR_1 & 0xF0) >> 4];
	h = chip8->IR_1 & 0x0F;

	chip8->V[0xF] = 0;

	for(int i=0; i < h; i++) {
		pixel = chip8->RAM[chip8->I + i];

		for(int j=0; j < 8; j++) {
			if( (pixel & (0x80 >> j) ) != 0) {
				if(chip8->f_buffer[x+j][y+i] != 0) {
					chip8->V[0xF] = 1;
				}
				chip8->f_buffer[x + j][y + i] ^= 1;
			}
		}
	}

	chip8->has_drawn = 1;
}

void CHIP8_Skip_If_Pressed(CHIP8* chip8) {
	if (chip8->keys[chip8->V[ chip8->IR_0 & 0x0F] ] == 1) {
		chip8->PC += 2;
	}
}

void CHIP8_Skip_IfNot_Pressed(CHIP8* chip8) {
	if (chip8->keys[chip8->V[ chip8->IR_0 & 0x0F] ] == 0) {
		chip8->PC += 2;
	}
}

void CHIP8_Wait_Key(CHIP8* chip8) {
	chip8->PC -= 2;

	for (int i = 0; i < 0xF; i++) {
		if (chip8->keys[i] != 0) {
			chip8->V[ chip8->IR_0 & 0x0F] = i;
			chip8->PC += 2;
		}
	}
	
}

void CHIP8_Set_VX_Delay(CHIP8* chip8) {
	chip8->V[chip8->IR_0 & 0x0F] = chip8->delay;
}

void CHIP8_Set_Delay_VX(CHIP8* chip8) {
	chip8->delay = chip8->V[chip8->IR_0 & 0x0F];
}

void CHIP8_Set_Sound_VX(CHIP8* chip8) {
	chip8->sound = chip8->V[chip8->IR_0 & 0x0F];
}

void CHIP8_Add_I_VX(CHIP8* chip8) {
	chip8->I += chip8->V[chip8->IR_0 & 0x0F];
}

void CHIP8_Set_I_Char(CHIP8* chip8) {
	chip8->I = FONT_BUFFER + chip8->V[chip8->IR_0 & 0x0F] * 5;
}

void CHIP8_Store_BCD(CHIP8* chip8) {
	u_int8_t vx = chip8->V[chip8->IR_0 & 0x0F];
	u_int8_t _0, _1;
	_0 = vx / 100;
	vx -= _0 * 100;
	_1 = vx / 10;
	vx -= _1 * 10;

	chip8->RAM[chip8->I + 0] = _0;
	chip8->RAM[chip8->I + 1] = _1;
	chip8->RAM[chip8->I + 2] = vx;
}

void CHIP8_Store_Registers(CHIP8* chip8) {
	u_int8_t vx = chip8->IR_0 & 0x0F;
	for (int i = 0; i <= vx; i++) {
		chip8->RAM[chip8->I + i] = chip8->V[i];
	}
}

void CHIP8_Load_Registers(CHIP8* chip8) {
	u_int8_t vx = chip8->IR_0 & 0x0F;
	for (int i = 0; i <= vx; i++) {
		chip8->V[i] = chip8->RAM[chip8->I + i];
	}
}

void CHIP8_Exec(CHIP8* chip8) {
	
	chip8->has_drawn = 0;
	chip8->IR_0 = *(u_int8_t*)(chip8->RAM + chip8->PC);
	chip8->IR_1 = *(u_int8_t*)(chip8->RAM + chip8->PC + 1);

	u_int8_t nibble_0 = chip8->IR_0 >> 4;
	u_int8_t nibble_3 = chip8->IR_1 & 0x0F;

	switch (nibble_0) {
	case 0x0:
		if (chip8->IR_1 == 0xE0) CHIP8_ClearScreen(chip8);
		else if (chip8->IR_1 == 0xEE) CHIP8_Return(chip8);
		break;
	case 0x1: CHIP8_GoTo(chip8); break;
	case 0x2: CHIP8_Call(chip8); break;
	case 0x3: CHIP8_SkipOnEqConst(chip8); break;
	case 0x4: CHIP8_SkipOnNotEqConst(chip8); break;
	case 0x5: CHIP8_SkipOnEqReg(chip8); break;
	case 0x6: CHIP8_MovConst(chip8); break;
	case 0x7: CHIP8_AddConst(chip8); break;
	case 0x8:
			switch (nibble_3) {
			case 0x0: CHIP8_Assign(chip8); break;
			case 0x1: CHIP8_OR(chip8); break;
			case 0x2: CHIP8_AND(chip8); break;
			case 0x3: CHIP8_XOR(chip8); break;
			case 0x4: CHIP8_ADD(chip8); break;
			case 0x5: CHIP8_SUB(chip8); break;
			case 0x6: CHIP8_BITR(chip8); break;
			case 0x7: CHIP8_SUBB(chip8); break;
			case 0xE: CHIP8_BITL(chip8); break;
			default:
				break;
			}
			break;
	case 0x9: CHIP8_SkipOnNotEqReg(chip8); break;
	case 0xA: CHIP8_MovI(chip8); break;
	case 0xB: CHIP8_GoToRelV0(chip8); break;
	case 0xC: CHIP8_MovRand(chip8); break;
	case 0xD: CHIP8_Draw(chip8); break;
	case 0xE: 
		switch (chip8->IR_1) {
			case 0x9E: CHIP8_Skip_If_Pressed(chip8); break;
			case 0xA1: CHIP8_Skip_IfNot_Pressed(chip8); break;
		}
		break;
	case 0xF: 
		switch (chip8->IR_1) {
			case 0x07: CHIP8_Set_VX_Delay(chip8); break;
			case 0x0A: CHIP8_Wait_Key(chip8); break;
			case 0x15: CHIP8_Set_Delay_VX(chip8); break;
			case 0x18: CHIP8_Set_Sound_VX(chip8); break;
			case 0x1E: CHIP8_Add_I_VX(chip8); break;
			case 0x29: CHIP8_Set_I_Char(chip8); break;
			case 0x33: CHIP8_Store_BCD(chip8); break;
			case 0x55: CHIP8_Store_Registers(chip8); break;
			case 0x65: CHIP8_Load_Registers(chip8); break;
		}
		break;
	default:
		break;
	}

	chip8->PC += 2;
}