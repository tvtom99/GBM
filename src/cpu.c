/*
	NAME: TMC
	INIT DATE: 14/01/2024
	LAST EDIT DATE: 16/01/2024
	DESC:
		This file is the main CPU. It will handle CPU ticks, executing opcodes, etc etc.

		This file includes an instruction struct that identifies every instruction the CPU can take. It includes some human-
		readable information to aide in future debugging. It also includes an 'instruction ticks' lookup table to aide the
		emulator in rechieving the expected execution time for each opcode.

		The instruction struct & the time lookup were originally taken from the open-source
		Cinoop emulator.
*/

#include "../include/cpu.h"
#include "../include/registers.h"
#include "../include/memory.h"
#include "../include/main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct registers registers;

const struct instruction instructions[256] = {
	{"NOP", 0, undefined},					   // 0x00
	{"LD BC, 0x%04X", 2, undefined},		   // 0x01
	{"LD (BC), A", 0, undefined},			   // 0x02
	{"INC BC", 0, undefined},				   // 0x03
	{"INC B", 0, undefined},				   // 0x04
	{"DEC B", 0, undefined},				   // 0x05
	{"LD B, 0x%02X", 1, undefined},			   // 0x06
	{"RLCA", 0, undefined},					   // 0x07
	{"LD (0x%04X), SP", 2, undefined},		   // 0x08
	{"ADD HL, BC", 0, undefined},			   // 0x09
	{"LD A, (BC)", 0, undefined},			   // 0x0a
	{"DEC BC", 0, undefined},				   // 0x0b
	{"INC C", 0, undefined},				   // 0x0c
	{"DEC C", 0, undefined},				   // 0x0d
	{"LD C, 0x%02X", 1, undefined},			   // 0x0e
	{"RRCA", 0, undefined},					   // 0x0f
	{"STOP", 1, undefined},					   // 0x10
	{"LD DE, 0x%04X", 2, undefined},		   // 0x11
	{"LD (DE), A", 0, undefined},			   // 0x12
	{"INC DE", 0, undefined},				   // 0x13
	{"INC D", 0, undefined},				   // 0x14
	{"DEC D", 0, undefined},				   // 0x15
	{"LD D, 0x%02X", 1, undefined},			   // 0x16
	{"RLA", 0, undefined},					   // 0x17
	{"JR 0x%02X", 1, undefined},			   // 0x18
	{"ADD HL, DE", 0, undefined},			   // 0x19
	{"LD A, (DE)", 0, undefined},			   // 0x1a
	{"DEC DE", 0, undefined},				   // 0x1b
	{"INC E", 0, undefined},				   // 0x1c
	{"DEC E", 0, undefined},				   // 0x1d
	{"LD E, 0x%02X", 1, undefined},			   // 0x1e
	{"RRA", 0, undefined},					   // 0x1f
	{"JR NZ, 0x%02X", 1, undefined},		   // 0x20
	{"LD HL, 0x%04X", 2, undefined},		   // 0x21
	{"LDI (HL), A", 0, undefined},			   // 0x22
	{"INC HL", 0, undefined},				   // 0x23
	{"INC H", 0, undefined},				   // 0x24
	{"DEC H", 0, undefined},				   // 0x25
	{"LD H, 0x%02X", 1, undefined},			   // 0x26
	{"DAA", 0, undefined},					   // 0x27
	{"JR Z, 0x%02X", 1, undefined},			   // 0x28
	{"ADD HL, HL", 0, undefined},			   // 0x29
	{"LDI A, (HL)", 0, undefined},			   // 0x2a
	{"DEC HL", 0, undefined},				   // 0x2b
	{"INC L", 0, undefined},				   // 0x2c
	{"DEC L", 0, undefined},				   // 0x2d
	{"LD L, 0x%02X", 1, undefined},			   // 0x2e
	{"CPL", 0, undefined},					   // 0x2f
	{"JR NC, 0x%02X", 1, undefined},		   // 0x30
	{"LD SP, 0x%04X", 2, undefined},		   // 0x31
	{"LDD (HL), A", 0, undefined},			   // 0x32
	{"INC SP", 0, undefined},				   // 0x33
	{"INC (HL)", 0, undefined},				   // 0x34
	{"DEC (HL)", 0, undefined},				   // 0x35
	{"LD (HL), 0x%02X", 1, undefined},		   // 0x36
	{"SCF", 0, undefined},					   // 0x37
	{"JR C, 0x%02X", 1, undefined},			   // 0x38
	{"ADD HL, SP", 0, undefined},			   // 0x39
	{"LDD A, (HL)", 0, undefined},			   // 0x3a
	{"DEC SP", 0, undefined},				   // 0x3b
	{"INC A", 0, undefined},				   // 0x3c
	{"DEC A", 0, undefined},				   // 0x3d
	{"LD A, 0x%02X", 1, undefined},			   // 0x3e
	{"CCF", 0, undefined},					   // 0x3f
	{"LD B, B", 0, undefined},				   // 0x40
	{"LD B, C", 0, undefined},				   // 0x41
	{"LD B, D", 0, undefined},				   // 0x42
	{"LD B, E", 0, undefined},				   // 0x43
	{"LD B, H", 0, undefined},				   // 0x44
	{"LD B, L", 0, undefined},				   // 0x45
	{"LD B, (HL)", 0, undefined},			   // 0x46
	{"LD B, A", 0, undefined},				   // 0x47
	{"LD C, B", 0, undefined},				   // 0x48
	{"LD C, C", 0, undefined},				   // 0x49
	{"LD C, D", 0, undefined},				   // 0x4a
	{"LD C, E", 0, undefined},				   // 0x4b
	{"LD C, H", 0, undefined},				   // 0x4c
	{"LD C, L", 0, undefined},				   // 0x4d
	{"LD C, (HL)", 0, undefined},			   // 0x4e
	{"LD C, A", 0, undefined},				   // 0x4f
	{"LD D, B", 0, undefined},				   // 0x50
	{"LD D, C", 0, undefined},				   // 0x51
	{"LD D, D", 0, undefined},				   // 0x52
	{"LD D, E", 0, undefined},				   // 0x53
	{"LD D, H", 0, undefined},				   // 0x54
	{"LD D, L", 0, undefined},				   // 0x55
	{"LD D, (HL)", 0, undefined},			   // 0x56
	{"LD D, A", 0, undefined},				   // 0x57
	{"LD E, B", 0, undefined},				   // 0x58
	{"LD E, C", 0, undefined},				   // 0x59
	{"LD E, D", 0, undefined},				   // 0x5a
	{"LD E, E", 0, undefined},				   // 0x5b
	{"LD E, H", 0, undefined},				   // 0x5c
	{"LD E, L", 0, undefined},				   // 0x5d
	{"LD E, (HL)", 0, undefined},			   // 0x5e
	{"LD E, A", 0, undefined},				   // 0x5f
	{"LD H, B", 0, undefined},				   // 0x60
	{"LD H, C", 0, undefined},				   // 0x61
	{"LD H, D", 0, undefined},				   // 0x62
	{"LD H, E", 0, undefined},				   // 0x63
	{"LD H, H", 0, undefined},				   // 0x64
	{"LD H, L", 0, undefined},				   // 0x65
	{"LD H, (HL)", 0, undefined},			   // 0x66
	{"LD H, A", 0, undefined},				   // 0x67
	{"LD L, B", 0, undefined},				   // 0x68
	{"LD L, C", 0, undefined},				   // 0x69
	{"LD L, D", 0, undefined},				   // 0x6a
	{"LD L, E", 0, undefined},				   // 0x6b
	{"LD L, H", 0, undefined},				   // 0x6c
	{"LD L, L", 0, undefined},				   // 0x6d
	{"LD L, (HL)", 0, undefined},			   // 0x6e
	{"LD L, A", 0, undefined},				   // 0x6f
	{"LD (HL), B", 0, undefined},			   // 0x70
	{"LD (HL), C", 0, undefined},			   // 0x71
	{"LD (HL), D", 0, undefined},			   // 0x72
	{"LD (HL), E", 0, undefined},			   // 0x73
	{"LD (HL), H", 0, undefined},			   // 0x74
	{"LD (HL), L", 0, undefined},			   // 0x75
	{"HALT", 0, undefined},					   // 0x76
	{"LD (HL), A", 0, undefined},			   // 0x77
	{"LD A, B", 0, undefined},				   // 0x78
	{"LD A, C", 0, undefined},				   // 0x79
	{"LD A, D", 0, undefined},				   // 0x7a
	{"LD A, E", 0, undefined},				   // 0x7b
	{"LD A, H", 0, undefined},				   // 0x7c
	{"LD A, L", 0, undefined},				   // 0x7d
	{"LD A, (HL)", 0, undefined},			   // 0x7e
	{"LD A, A", 0, undefined},				   // 0x7f
	{"ADD A, B", 0, undefined},				   // 0x80
	{"ADD A, C", 0, undefined},				   // 0x81
	{"ADD A, D", 0, undefined},				   // 0x82
	{"ADD A, E", 0, undefined},				   // 0x83
	{"ADD A, H", 0, undefined},				   // 0x84
	{"ADD A, L", 0, undefined},				   // 0x85
	{"ADD A, (HL)", 0, undefined},			   // 0x86
	{"ADD A", 0, undefined},				   // 0x87
	{"ADC B", 0, undefined},				   // 0x88
	{"ADC C", 0, undefined},				   // 0x89
	{"ADC D", 0, undefined},				   // 0x8a
	{"ADC E", 0, undefined},				   // 0x8b
	{"ADC H", 0, undefined},				   // 0x8c
	{"ADC L", 0, undefined},				   // 0x8d
	{"ADC (HL)", 0, undefined},				   // 0x8e
	{"ADC A", 0, undefined},				   // 0x8f
	{"SUB B", 0, undefined},				   // 0x90
	{"SUB C", 0, undefined},				   // 0x91
	{"SUB D", 0, undefined},				   // 0x92
	{"SUB E", 0, undefined},				   // 0x93
	{"SUB H", 0, undefined},				   // 0x94
	{"SUB L", 0, undefined},				   // 0x95
	{"SUB (HL)", 0, undefined},				   // 0x96
	{"SUB A", 0, undefined},				   // 0x97
	{"SBC B", 0, undefined},				   // 0x98
	{"SBC C", 0, undefined},				   // 0x99
	{"SBC D", 0, undefined},				   // 0x9a
	{"SBC E", 0, undefined},				   // 0x9b
	{"SBC H", 0, undefined},				   // 0x9c
	{"SBC L", 0, undefined},				   // 0x9d
	{"SBC (HL)", 0, undefined},				   // 0x9e
	{"SBC A", 0, undefined},				   // 0x9f
	{"AND B", 0, undefined},				   // 0xa0
	{"AND C", 0, undefined},				   // 0xa1
	{"AND D", 0, undefined},				   // 0xa2
	{"AND E", 0, undefined},				   // 0xa3
	{"AND H", 0, undefined},				   // 0xa4
	{"AND L", 0, undefined},				   // 0xa5
	{"AND (HL)", 0, undefined},				   // 0xa6
	{"AND A", 0, undefined},				   // 0xa7
	{"XOR B", 0, undefined},				   // 0xa8
	{"XOR C", 0, undefined},				   // 0xa9
	{"XOR D", 0, undefined},				   // 0xaa
	{"XOR E", 0, undefined},				   // 0xab
	{"XOR H", 0, undefined},				   // 0xac
	{"XOR L", 0, undefined},				   // 0xad
	{"XOR (HL)", 0, undefined},				   // 0xae
	{"XOR A", 0, undefined},				   // 0xaf
	{"OR B", 0, undefined},					   // 0xb0
	{"OR C", 0, undefined},					   // 0xb1
	{"OR D", 0, undefined},					   // 0xb2
	{"OR E", 0, undefined},					   // 0xb3
	{"OR H", 0, undefined},					   // 0xb4
	{"OR L", 0, undefined},					   // 0xb5
	{"OR (HL)", 0, undefined},				   // 0xb6
	{"OR A", 0, undefined},					   // 0xb7
	{"CP B", 0, undefined},					   // 0xb8
	{"CP C", 0, undefined},					   // 0xb9
	{"CP D", 0, undefined},					   // 0xba
	{"CP E", 0, undefined},					   // 0xbb
	{"CP H", 0, undefined},					   // 0xbc
	{"CP L", 0, undefined},					   // 0xbd
	{"CP (HL)", 0, undefined},				   // 0xbe
	{"CP A", 0, undefined},					   // 0xbf
	{"RET NZ", 0, undefined},				   // 0xc0
	{"POP BC", 0, undefined},				   // 0xc1
	{"JP NZ, 0x%04X", 2, undefined},		   // 0xc2
	{"JP 0x%04X", 2, undefined},			   // 0xc3
	{"CALL NZ, 0x%04X", 2, undefined},		   // 0xc4
	{"PUSH BC", 0, undefined},				   // 0xc5
	{"ADD A, 0x%02X", 1, undefined},		   // 0xc6
	{"RST 0x00", 0, undefined},				   // 0xc7
	{"RET Z", 0, undefined},				   // 0xc8
	{"RET", 0, undefined},					   // 0xc9
	{"JP Z, 0x%04X", 2, undefined},			   // 0xca
	{"CB %02X", 1, undefined},				   // 0xcb
	{"CALL Z, 0x%04X", 2, undefined},		   // 0xcc
	{"CALL 0x%04X", 2, undefined},			   // 0xcd
	{"ADC 0x%02X", 1, undefined},			   // 0xce
	{"RST 0x08", 0, undefined},				   // 0xcf
	{"RET NC", 0, undefined},				   // 0xd0
	{"POP DE", 0, undefined},				   // 0xd1
	{"JP NC, 0x%04X", 2, undefined},		   // 0xd2
	{"UNKNOWN", 0, undefined},				   // 0xd3
	{"CALL NC, 0x%04X", 2, undefined},		   // 0xd4
	{"PUSH DE", 0, undefined},				   // 0xd5
	{"SUB 0x%02X", 1, undefined},			   // 0xd6
	{"RST 0x10", 0, undefined},				   // 0xd7
	{"RET C", 0, undefined},				   // 0xd8
	{"RETI", 0, undefined},					   // 0xd9
	{"JP C, 0x%04X", 2, undefined},			   // 0xda
	{"UNKNOWN", 0, undefined},				   // 0xdb
	{"CALL C, 0x%04X", 2, undefined},		   // 0xdc
	{"UNKNOWN", 0, undefined},				   // 0xdd
	{"SBC 0x%02X", 1, undefined},			   // 0xde
	{"RST 0x18", 0, undefined},				   // 0xdf
	{"LD (0xFF00 + 0x%02X), A", 1, undefined}, // 0xe0
	{"POP HL", 0, undefined},				   // 0xe1
	{"LD (0xFF00 + C), A", 0, undefined},	   // 0xe2
	{"UNKNOWN", 0, undefined},				   // 0xe3
	{"UNKNOWN", 0, undefined},				   // 0xe4
	{"PUSH HL", 0, undefined},				   // 0xe5
	{"AND 0x%02X", 1, undefined},			   // 0xe6
	{"RST 0x20", 0, undefined},				   // 0xe7
	{"ADD SP,0x%02X", 1, undefined},		   // 0xe8
	{"JP HL", 0, undefined},				   // 0xe9
	{"LD (0x%04X), A", 2, undefined},		   // 0xea
	{"UNKNOWN", 0, undefined},				   // 0xeb
	{"UNKNOWN", 0, undefined},				   // 0xec
	{"UNKNOWN", 0, undefined},				   // 0xed
	{"XOR 0x%02X", 1, undefined},			   // 0xee
	{"RST 0x28", 0, undefined},				   // 0xef
	{"LD A, (0xFF00 + 0x%02X)", 1, undefined}, // 0xf0
	{"POP AF", 0, undefined},				   // 0xf1
	{"LD A, (0xFF00 + C)", 0, undefined},	   // 0xf2
	{"DI", 0, undefined},					   // 0xf3
	{"UNKNOWN", 0, undefined},				   // 0xf4
	{"PUSH AF", 0, undefined},				   // 0xf5
	{"OR 0x%02X", 1, undefined},			   // 0xf6
	{"RST 0x30", 0, undefined},				   // 0xf7
	{"LD HL, SP+0x%02X", 1, undefined},		   // 0xf8
	{"LD SP, HL", 0, undefined},			   // 0xf9
	{"LD A, (0x%04X)", 2, undefined},		   // 0xfa
	{"EI", 0, undefined},					   // 0xfb
	{"UNKNOWN", 0, undefined},				   // 0xfc
	{"UNKNOWN", 0, undefined},				   // 0xfd
	{"CP 0x%02X", 1, undefined},			   // 0xfe
	{"RST 0x38", 0, undefined},				   // 0xff
};

const unsigned char instructionTicks[256] = {
	2, 6, 4, 4, 2, 2, 4, 4, 10, 4, 4, 4, 2, 2, 4, 4, // 0x0_
	2, 6, 4, 4, 2, 2, 4, 4, 4, 4, 4, 4, 2, 2, 4, 4,	 // 0x1_
	0, 6, 4, 4, 2, 2, 4, 2, 0, 4, 4, 4, 2, 2, 4, 2,	 // 0x2_
	4, 6, 4, 4, 6, 6, 6, 2, 0, 4, 4, 4, 2, 2, 4, 2,	 // 0x3_
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,	 // 0x4_
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,	 // 0x5_
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,	 // 0x6_
	4, 4, 4, 4, 4, 4, 2, 4, 2, 2, 2, 2, 2, 2, 4, 2,	 // 0x7_
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,	 // 0x8_
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,	 // 0x9_
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,	 // 0xa_
	2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,	 // 0xb_
	0, 6, 0, 6, 0, 8, 4, 8, 0, 2, 0, 0, 0, 6, 4, 8,	 // 0xc_
	0, 6, 0, 0, 0, 8, 4, 8, 0, 8, 0, 0, 0, 0, 4, 8,	 // 0xd_
	6, 6, 4, 0, 0, 8, 4, 8, 8, 2, 8, 0, 0, 0, 4, 8,	 // 0xe_
	6, 6, 4, 2, 0, 8, 4, 8, 6, 4, 8, 2, 0, 0, 4, 8	 // 0xf_
};

void reset(void)
{
	// Initialise all memory locations
	/*
		IMPORTANT: RAM on physical GB is not actually initialised to 0 on startup. Instead,
		it is set to random values. It is expected that the programmer will set it to
		whatever values are needed when necessary. I will set it to 0 cause it's easier
		than setting random values throughout it, and I don't think it'll have a negative
		effect.
	*/

	printf("Initialising reset...\n"); // DEBUG

	memset(sram, 0, sizeof(sram));
	memcpy(io, ioReset, sizeof(io));
	memset(vram, 0, sizeof(vram));
	memset(oam, 0, sizeof(0));
	memset(wram, 0, sizeof(wram));
	memset(hram, 0, sizeof(hram));

	// Initialise the registers as per CPU guide
	registers.pc = 0x100;
	registers.sp = 0xFFFE;
	registers.a = 0x01; // I've split these up to better understand what's happening.
	registers.f = 0xB0; // In the guide, it states 'AF set to $01 on GB/SGB', but then
						//'F set to 0xB0'? I think this is what it wants done.
	registers.bc = 0x0013;
	registers.de = 0x00D8;
	registers.hl = 0x014D;

	// Initialise the stack pointer & program counter.
	registers.sp = 0xfffe;
	registers.pc = 0x100;

	/*
		INITIAL BYTE WRITES:
			[$FF05] = $00 ; TIMA
			[$FF06] = $00 ; TMA
			[$FF07] = $00 ; TAC
			[$FF10] = $80 ; NR10
			[$FF11] = $BF ; NR11
			[$FF12] = $F3 ; NR12
			[$FF14] = $BF ; NR14
			[$FF16] = $3F ; NR21
			[$FF17] = $00 ; NR22
			[$FF19] = $BF ; NR24
			[$FF1A] = $7F ; NR30
			[$FF1B] = $FF ; NR31
			[$FF1C] = $9F ; NR32
			[$FF1E] = $BF ; NR33
			[$FF20] = $FF ; NR41
			[$FF21] = $00 ; NR42
			[$FF22] = $00 ; NR43
			[$FF23] = $BF ; NR30
			[$FF24] = $77 ; NR50
			[$FF25] = $F3 ; NR51
			[$FF26] = $F1-GB, $F0-SGB ; NR52
			[$FF40] = $91 ; LCDC
			[$FF42] = $00 ; SCY
			[$FF43] = $00 ; SCX
			[$FF45] = $00 ; LYC
			[$FF47] = $FC ; BGP
			[$FF48] = $FF ; OBP0
			[$FF49] = $FF ; OBP1
			[$FF4A] = $00 ; WY
			[$FF4B] = $00 ; WX
			[$FFFF] = $00 ; IE

			This displays the gameboy intro logo
	*/

	writeByte(0xFF05, 0x00);
	writeByte(0xFF06, 0x00);
	writeByte(0xFF07, 0x00);
	writeByte(0xFF10, 0x80);
	writeByte(0xFF11, 0xBF);
	writeByte(0xFF12, 0xF3);
	writeByte(0xFF14, 0xBF);
	writeByte(0xFF16, 0x3F);
	writeByte(0xFF17, 0x00);
	writeByte(0xFF19, 0xBF);
	writeByte(0xFF1A, 0x7F);
	writeByte(0xFF1B, 0xFF);
	writeByte(0xFF1C, 0x9F);
	writeByte(0xFF1E, 0xBF);
	writeByte(0xFF20, 0xFF);
	writeByte(0xFF21, 0x00);
	writeByte(0xFF22, 0x00);
	writeByte(0xFF23, 0xBF);
	writeByte(0xFF24, 0x77);
	writeByte(0xFF25, 0xF3);
	writeByte(0xFF26, 0xF1);
	writeByte(0xFF40, 0x91);
	writeByte(0xFF42, 0x00);
	writeByte(0xFF43, 0x00);
	writeByte(0xFF45, 0x00);
	writeByte(0xFF47, 0xFC);
	writeByte(0xFF48, 0xFF);
	writeByte(0xFF49, 0xFF);
	writeByte(0xFF4A, 0x00);
	writeByte(0xFF4B, 0x00);
	writeByte(0xFFFF, 0x00);

	printf("Finished reset!\n"); // DEBUG
}

void stepCPU()
{
	unsigned char instruction;
	unsigned short operand = 0;

	// Get instruction & increment pc
	printf("Program Counter is currently at: 0x%x.\n", registers.pc);
	instruction = readByte(registers.pc++);
	printf("Reading instruction 0x%x...\n", instruction);

	// Get the opperand of the function (if any).
	printf("Checking opperand length...\n");
	if (instructions[instruction].operandLength == 1)
	{
		operand = (unsigned short)readByte(registers.pc);
	}
	if (instructions[instruction].operandLength == 2)
	{
		operand = readShort(registers.pc);
	}

	// Increment pc by the length of the opperand length (only occurs if there is an opperand)
	printf("Incrementing pc if opperand found...\n");
	registers.pc += instructions[instruction].operandLength;

	// Select opperation to execute! (taken directly from Cinoop for now.)
	// It appears each case dereferences a pointer to a function which is stored in the 'instructions' array.
	printf("Executing instruction 0x%x!\n", instruction);

	switch (instructions[instruction].operandLength)
	{
	case 0:
		((void (*)(void))instructions[instruction].execute)();
		break;

	case 1:
		((void (*)(unsigned char))instructions[instruction].execute)((unsigned char)operand);
		break;

	case 2:
		((void (*)(unsigned short))instructions[instruction].execute)(operand);
		break;
	}
}

void undefined(void)
{
	registers.pc--;

	unsigned char instruction = readByte(registers.pc);

	printf("\n===============\nUndefined instruction 0x%02x!\n\nRegisters:\n", instruction);
	printf("A: 0x%02x\n", registers.a);
	printf("F: 0x%02x\n", registers.f);
	printf("B: 0x%02x\n", registers.b);
	printf("C: 0x%02x\n", registers.c);
	printf("D: 0x%02x\n", registers.d);
	printf("E: 0x%02x\n", registers.e);
	printf("H: 0x%02x\n", registers.h);
	printf("L: 0x%02x\n", registers.l);
	printf("SP: 0x%04x\n", registers.sp);
	printf("PC: 0x%04x\n", registers.pc);
	printf("===============\n\n");

	quit();
}