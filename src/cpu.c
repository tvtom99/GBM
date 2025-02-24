/*
	NAME: TMC
	INIT DATE: 14/01/2024
	LAST EDIT DATE: 20/01/2024
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
#include "../include/debug.h"
#include "../include/interupts.h"
#include "../include/keys.h"
#include "../include/gpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct registers registers;
unsigned int lastOpperand;

const struct instruction instructions[256] = {
	{"NOP", 0, nop},							// 0x00
	{"LD BC, 0x%04X", 2, ld_bc_nn},				// 0x01
	{"LD (BC), A", 0, ld_bcp_a},				// 0x02
	{"INC BC", 0, undefined},					// 0x03
	{"INC B", 0, undefined},					// 0x04
	{"DEC B", 0, dec_b},						// 0x05
	{"LD B, 0x%02X", 1, ld_b_n},				// 0x06
	{"RLCA", 0, undefined},						// 0x07
	{"LD (0x%04X), SP", 2, undefined},			// 0x08
	{"ADD HL, BC", 0, undefined},				// 0x09
	{"LD A, (BC)", 0, undefined},				// 0x0a
	{"DEC BC", 0, dec_bc},						// 0x0b
	{"INC C", 0, inc_c},						// 0x0c
	{"DEC C", 0, dec_c},						// 0x0d
	{"LD C, 0x%02X", 1, ld_c_n},				// 0x0e
	{"RRCA", 0, undefined},						// 0x0f
	{"STOP", 1, undefined},						// 0x10
	{"LD DE, 0x%04X", 2, ld_de_nn},				// 0x11
	{"LD (DE), A", 0, undefined},				// 0x12
	{"INC DE", 0, undefined},					// 0x13
	{"INC D", 0, undefined},					// 0x14
	{"DEC D", 0, undefined},					// 0x15
	{"LD D, 0x%02X", 1, undefined},				// 0x16
	{"RLA", 0, undefined},						// 0x17
	{"JR 0x%02X", 1, undefined},				// 0x18
	{"ADD HL, DE", 0, undefined},				// 0x19
	{"LD A, (DE)", 0, undefined},				// 0x1a
	{"DEC DE", 0, undefined},					// 0x1b
	{"INC E", 0, undefined},					// 0x1c
	{"DEC E", 0, undefined},					// 0x1d
	{"LD E, 0x%02X", 1, undefined},				// 0x1e
	{"RRA", 0, undefined},						// 0x1f
	{"JR NZ, 0x%02X", 1, jr_nz_n},				// 0x20
	{"LD HL, 0x%04X", 2, ld_hl_nn},				// 0x21
	{"LDI (HL), A", 0, undefined},				// 0x22
	{"INC HL", 0, undefined},					// 0x23
	{"INC H", 0, undefined},					// 0x24
	{"DEC H", 0, undefined},					// 0x25
	{"LD H, 0x%02X", 1, undefined},				// 0x26
	{"DAA", 0, undefined},						// 0x27
	{"JR Z, 0x%02X", 1, undefined},				// 0x28
	{"ADD HL, HL", 0, undefined},				// 0x29
	{"LDI A, (HL)", 0, ldi_a_hlp},				// 0x2a
	{"DEC HL", 0, undefined},					// 0x2b
	{"INC L", 0, undefined},					// 0x2c
	{"DEC L", 0, undefined},					// 0x2d
	{"LD L, 0x%02X", 1, undefined},				// 0x2e
	{"CPL", 0, undefined},						// 0x2f
	{"JR NC, 0x%02X", 1, undefined},			// 0x30
	{"LD SP, 0x%04X", 2, ld_sp_nn},				// 0x31
	{"LDD (HL), A", 0, ldd_hlp_a},				// 0x32
	{"INC SP", 0, undefined},					// 0x33
	{"INC (HL)", 0, undefined},					// 0x34
	{"DEC (HL)", 0, undefined},					// 0x35
	{"LD (HL), 0x%02X", 1, ld_hlp_n},			// 0x36
	{"SCF", 0, undefined},						// 0x37
	{"JR C, 0x%02X", 1, undefined},				// 0x38
	{"ADD HL, SP", 0, undefined},				// 0x39
	{"LDD A, (HL)", 0, undefined},				// 0x3a
	{"DEC SP", 0, undefined},					// 0x3b
	{"INC A", 0, undefined},					// 0x3c
	{"DEC A", 0, undefined},					// 0x3d
	{"LD A, 0x%02X", 1, ld_a_n},				// 0x3e
	{"CCF", 0, undefined},						// 0x3f
	{"LD B, B", 0, undefined},					// 0x40
	{"LD B, C", 0, undefined},					// 0x41
	{"LD B, D", 0, undefined},					// 0x42
	{"LD B, E", 0, undefined},					// 0x43
	{"LD B, H", 0, undefined},					// 0x44
	{"LD B, L", 0, undefined},					// 0x45
	{"LD B, (HL)", 0, undefined},				// 0x46
	{"LD B, A", 0, ld_b_a},						// 0x47
	{"LD C, B", 0, undefined},					// 0x48
	{"LD C, C", 0, undefined},					// 0x49
	{"LD C, D", 0, undefined},					// 0x4a
	{"LD C, E", 0, undefined},					// 0x4b
	{"LD C, H", 0, undefined},					// 0x4c
	{"LD C, L", 0, undefined},					// 0x4d
	{"LD C, (HL)", 0, undefined},				// 0x4e
	{"LD C, A", 0, undefined},					// 0x4f
	{"LD D, B", 0, undefined},					// 0x50
	{"LD D, C", 0, undefined},					// 0x51
	{"LD D, D", 0, undefined},					// 0x52
	{"LD D, E", 0, undefined},					// 0x53
	{"LD D, H", 0, undefined},					// 0x54
	{"LD D, L", 0, undefined},					// 0x55
	{"LD D, (HL)", 0, undefined},				// 0x56
	{"LD D, A", 0, undefined},					// 0x57
	{"LD E, B", 0, undefined},					// 0x58
	{"LD E, C", 0, undefined},					// 0x59
	{"LD E, D", 0, undefined},					// 0x5a
	{"LD E, E", 0, undefined},					// 0x5b
	{"LD E, H", 0, undefined},					// 0x5c
	{"LD E, L", 0, undefined},					// 0x5d
	{"LD E, (HL)", 0, undefined},				// 0x5e
	{"LD E, A", 0, undefined},					// 0x5f
	{"LD H, B", 0, undefined},					// 0x60
	{"LD H, C", 0, undefined},					// 0x61
	{"LD H, D", 0, undefined},					// 0x62
	{"LD H, E", 0, undefined},					// 0x63
	{"LD H, H", 0, undefined},					// 0x64
	{"LD H, L", 0, undefined},					// 0x65
	{"LD H, (HL)", 0, undefined},				// 0x66
	{"LD H, A", 0, undefined},					// 0x67
	{"LD L, B", 0, undefined},					// 0x68
	{"LD L, C", 0, undefined},					// 0x69
	{"LD L, D", 0, undefined},					// 0x6a
	{"LD L, E", 0, undefined},					// 0x6b
	{"LD L, H", 0, undefined},					// 0x6c
	{"LD L, L", 0, undefined},					// 0x6d
	{"LD L, (HL)", 0, undefined},				// 0x6e
	{"LD L, A", 0, undefined},					// 0x6f
	{"LD (HL), B", 0, undefined},				// 0x70
	{"LD (HL), C", 0, undefined},				// 0x71
	{"LD (HL), D", 0, undefined},				// 0x72
	{"LD (HL), E", 0, undefined},				// 0x73
	{"LD (HL), H", 0, undefined},				// 0x74
	{"LD (HL), L", 0, undefined},				// 0x75
	{"HALT", 0, undefined},						// 0x76
	{"LD (HL), A", 0, undefined},				// 0x77
	{"LD A, B", 0, ld_a_b},						// 0x78
	{"LD A, C", 0, undefined},					// 0x79
	{"LD A, D", 0, undefined},					// 0x7a
	{"LD A, E", 0, undefined},					// 0x7b
	{"LD A, H", 0, undefined},					// 0x7c
	{"LD A, L", 0, undefined},					// 0x7d
	{"LD A, (HL)", 0, undefined},				// 0x7e
	{"LD A, A", 0, undefined},					// 0x7f
	{"ADD A, B", 0, undefined},					// 0x80
	{"ADD A, C", 0, undefined},					// 0x81
	{"ADD A, D", 0, undefined},					// 0x82
	{"ADD A, E", 0, undefined},					// 0x83
	{"ADD A, H", 0, undefined},					// 0x84
	{"ADD A, L", 0, undefined},					// 0x85
	{"ADD A, (HL)", 0, undefined},				// 0x86
	{"ADD A", 0, undefined},					// 0x87
	{"ADC B", 0, undefined},					// 0x88
	{"ADC C", 0, undefined},					// 0x89
	{"ADC D", 0, undefined},					// 0x8a
	{"ADC E", 0, undefined},					// 0x8b
	{"ADC H", 0, undefined},					// 0x8c
	{"ADC L", 0, undefined},					// 0x8d
	{"ADC (HL)", 0, undefined},					// 0x8e
	{"ADC A", 0, undefined},					// 0x8f
	{"SUB B", 0, undefined},					// 0x90
	{"SUB C", 0, undefined},					// 0x91
	{"SUB D", 0, undefined},					// 0x92
	{"SUB E", 0, undefined},					// 0x93
	{"SUB H", 0, undefined},					// 0x94
	{"SUB L", 0, undefined},					// 0x95
	{"SUB (HL)", 0, undefined},					// 0x96
	{"SUB A", 0, undefined},					// 0x97
	{"SBC B", 0, undefined},					// 0x98
	{"SBC C", 0, undefined},					// 0x99
	{"SBC D", 0, undefined},					// 0x9a
	{"SBC E", 0, undefined},					// 0x9b
	{"SBC H", 0, undefined},					// 0x9c
	{"SBC L", 0, undefined},					// 0x9d
	{"SBC (HL)", 0, undefined},					// 0x9e
	{"SBC A", 0, undefined},					// 0x9f
	{"AND B", 0, undefined},					// 0xa0
	{"AND C", 0, undefined},					// 0xa1
	{"AND D", 0, undefined},					// 0xa2
	{"AND E", 0, and_e},						// 0xa3
	{"AND H", 0, undefined},					// 0xa4
	{"AND L", 0, undefined},					// 0xa5
	{"AND (HL)", 0, undefined},					// 0xa6
	{"AND A", 0, undefined},					// 0xa7
	{"XOR B", 0, undefined},					// 0xa8
	{"XOR C", 0, undefined},					// 0xa9
	{"XOR D", 0, undefined},					// 0xaa
	{"XOR E", 0, undefined},					// 0xab
	{"XOR H", 0, undefined},					// 0xac
	{"XOR L", 0, undefined},					// 0xad
	{"XOR (HL)", 0, undefined},					// 0xae
	{"XOR A", 0, xor_a},						// 0xaf
	{"OR B", 0, undefined},						// 0xb0
	{"OR C", 0, or_c},							// 0xb1
	{"OR D", 0, undefined},						// 0xb2
	{"OR E", 0, undefined},						// 0xb3
	{"OR H", 0, undefined},						// 0xb4
	{"OR L", 0, undefined},						// 0xb5
	{"OR (HL)", 0, undefined},					// 0xb6
	{"OR A", 0, undefined},						// 0xb7
	{"CP B", 0, undefined},						// 0xb8
	{"CP C", 0, undefined},						// 0xb9
	{"CP D", 0, undefined},						// 0xba
	{"CP E", 0, undefined},						// 0xbb
	{"CP H", 0, undefined},						// 0xbc
	{"CP L", 0, undefined},						// 0xbd
	{"CP (HL)", 0, undefined},					// 0xbe
	{"CP A", 0, undefined},						// 0xbf
	{"RET NZ", 0, undefined},					// 0xc0
	{"POP BC", 0, undefined},					// 0xc1
	{"JP NZ, 0x%04X", 2, undefined},			// 0xc2
	{"JP 0x%04X", 2, jp_nn},					// 0xc3
	{"CALL NZ, 0x%04X", 2, undefined},			// 0xc4
	{"PUSH BC", 0, undefined},					// 0xc5
	{"ADD A, 0x%02X", 1, undefined},			// 0xc6
	{"RST 0x00", 0, undefined},					// 0xc7
	{"RET Z", 0, undefined},					// 0xc8
	{"RET", 0, ret},							// 0xc9
	{"JP Z, 0x%04X", 2, undefined},				// 0xca
	{"CB %02X", 1, undefined},					// 0xcb
	{"CALL Z, 0x%04X", 2, undefined},			// 0xcc
	{"CALL 0x%04X", 2, call_nn},				// 0xcd
	{"ADC 0x%02X", 1, undefined},				// 0xce
	{"RST 0x08", 0, undefined},					// 0xcf
	{"RET NC", 0, undefined},					// 0xd0
	{"POP DE", 0, undefined},					// 0xd1
	{"JP NC, 0x%04X", 2, undefined},			// 0xd2
	{"UNKNOWN", 0, undefined},					// 0xd3
	{"CALL NC, 0x%04X", 2, undefined},			// 0xd4
	{"PUSH DE", 0, undefined},					// 0xd5
	{"SUB 0x%02X", 1, undefined},				// 0xd6
	{"RST 0x10", 0, undefined},					// 0xd7
	{"RET C", 0, undefined},					// 0xd8
	{"RETI", 0, undefined},						// 0xd9
	{"JP C, 0x%04X", 2, undefined},				// 0xda
	{"UNKNOWN", 0, undefined},					// 0xdb
	{"CALL C, 0x%04X", 2, undefined},			// 0xdc
	{"UNKNOWN", 0, undefined},					// 0xdd
	{"SBC 0x%02X", 1, undefined},				// 0xde
	{"RST 0x18", 0, rst_18},					// 0xdf
	{"LD (0xFF00 + 0x%02X), A", 1, ld_ff_n_ap}, // 0xe0
	{"POP HL", 0, undefined},					// 0xe1
	{"LD (0xFF00 + C), A", 0, ld_ff_c_a},		// 0xe2
	{"UNKNOWN", 0, undefined},					// 0xe3
	{"UNKNOWN", 0, undefined},					// 0xe4
	{"PUSH HL", 0, undefined},					// 0xe5
	{"AND 0x%02X", 1, undefined},				// 0xe6
	{"RST 0x20", 0, undefined},					// 0xe7
	{"ADD SP,0x%02X", 1, undefined},			// 0xe8
	{"JP HL", 0, undefined},					// 0xe9
	{"LD (0x%04X), A", 2, ld_nnp_a},			// 0xea
	{"UNKNOWN", 0, undefined},					// 0xeb
	{"UNKNOWN", 0, undefined},					// 0xec
	{"UNKNOWN", 0, undefined},					// 0xed
	{"XOR 0x%02X", 1, undefined},				// 0xee
	{"RST 0x28", 0, undefined},					// 0xef
	{"LD A, (0xFF00 + 0x%02X)", 1, ld_ff_ap_n}, // 0xf0
	{"POP AF", 0, undefined},					// 0xf1
	{"LD A, (0xFF00 + C)", 0, undefined},		// 0xf2
	{"DI", 0, di},								// 0xf3
	{"UNKNOWN", 0, undefined},					// 0xf4
	{"PUSH AF", 0, undefined},					// 0xf5
	{"OR 0x%02X", 1, undefined},				// 0xf6
	{"RST 0x30", 0, undefined},					// 0xf7
	{"LD HL, SP+0x%02X", 1, undefined},			// 0xf8
	{"LD SP, HL", 0, undefined},				// 0xf9
	{"LD A, (0x%04X)", 2, undefined},			// 0xfa
	{"EI", 0, undefined},						// 0xfb
	{"UNKNOWN", 0, undefined},					// 0xfc
	{"UNKNOWN", 0, undefined},					// 0xfd
	{"CP 0x%02X", 1, cp_n},						// 0xfe
	{"RST 0x38", 0, rst_38},					// 0xff
};

// This is a database of how many ticks each instruction should take.
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

unsigned long ticks;
unsigned char stopped;

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

	// Initialise the interrupts
	interrupt.master = 1;
	interrupt.enable = 0;
	interrupt.flags = 0;

	// Initialise the keys
	keys.a = 1;
	keys.b = 1;
	keys.select = 1;
	keys.start = 1;
	keys.right = 1;
	keys.left = 1;
	keys.up = 1;
	keys.down = 1;

	// Initialise the GPU
	gpu.control = 0;
	gpu.scrollX = 0;
	gpu.scrollY = 0;
	gpu.scanline = 0;
	gpu.tick = 0;

	// Initialise ticks and stopped variable
	ticks = 0;
	stopped = 0;

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

	printf("Finished reset!\n\n"); // DEBUG
}

void stepCPU()
{
	unsigned char instruction;
	unsigned short operand = 0;
	lastOpperand = 0; // DEBUG VALUE used for correctly moving pc back if unknown opcode encountered

	// Check stopped
	if (stopped)
	{
		// Do nothing
		return;
	}

	//BREAKPOINT DEBUG
	if(registers.pc == 0x21B || registers.pc == 0x219)
	{
		debugModeEnable = 1;
	}

	// Debug stuff
	if (debugModeEnable)
	{
		// Show pop-up of current execution

		showRealtimeData();
	}

	// Get instruction & increment pc
	// printf("Program Counter is currently at: 0x%x.\n", registers.pc);
	instruction = readByte(registers.pc++);
	// printf("Reading instruction 0x%02x...\n", instruction);

	// Get the opperand of the function (if any).
	// printf("Operand Length of: %u\n", instructions[instruction].operandLength);
	if (instructions[instruction].operandLength == 1)
	{
		operand = (unsigned short)readByte(registers.pc);
		// printf("Found opperand of: 0x%x\n", operand);
		lastOpperand = 1;
		// printf("Incrementing pc...\n");
	}
	if (instructions[instruction].operandLength == 2)
	{
		operand = readShort(registers.pc);
		// printf("Found opperand of: 0x%x\n", operand);
		lastOpperand = 2;
		// printf("Incrementing pc...\n");
	}

	// Increment pc by the length of the opperand length (only occurs if there is an opperand)
	registers.pc += instructions[instruction].operandLength;

	// Select opperation to execute! (taken directly from Cinoop for now.)
	// Each case dereferences a pointer to a function which is stored in the 'instructions' array.

	switch (instructions[instruction].operandLength)
	{
	case 0:
		// printf("Executing 0x%.02x, '%s'.\n", instruction, instructions[instruction].disassembly);
		((void (*)(void))instructions[instruction].execute)();
		break;

	case 1:
		// printf("Executing 0x%.02x, '%s'. Opperand is: 0x%02x.\n", instruction, instructions[instruction].disassembly, operand);
		((void (*)(unsigned char))instructions[instruction].execute)((unsigned char)operand);
		break;

	case 2:
		// printf("Executing 0x%.02x, '%s'. Opperand is: 0x%02x.\n", instruction, instructions[instruction].disassembly, operand);
		((void (*)(unsigned short))instructions[instruction].execute)(operand);
		break;
	}

	// Add the total amount of ticks this instruction would have taken to the total ticks
	ticks += instructionTicks[instruction];

	// printf("Finished CPU cycle!\n\n");

	// VERY DEBUG, MUST DELETE AFTER THIS BREAKPOINT IS REACHED. Cinoop did this so I wanna track when I hit it too >:)
	if (registers.pc == 0x2817)
	{
		printf("You've hit the point where vram starts to be accessed :O Stopping emulation so you can celebrate!");
		quit();
	}
}

void undefined(void)
{
	registers.pc -= lastOpperand + 1; // decrement pc by 1 + however long the last opperand was
									  // cause it would have increment the pc too

	unsigned char instruction = readByte(registers.pc);

	printf("\n===============\nUndefined instruction: 0x%02x!\nInstruction information: %s\n\nRegisters:\n", instruction, instructions[instruction].disassembly);
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

	// Print ROM contents for debug checking (DO NOT LEAVE IN FINAL RELEASE)
	// FILE *debugF = fopen("rom_output.txt", "w");
	// // fprintf(debugF, "%s\n", cart);
	// int j;
	// for (j = 0; j < (int)sizeof cart; j++)
	// {
	// 	fprintf(debugF, "0x%.02x\n", cart[j]);
	// }
	// fclose(debugF);

	quit();
}

/*===========================================
	INSTRUCTIONS HELPERS
	------
	Helpers for the instructions.
============================================*/

/*
	dec
	---
	When given a value, it will return that value MINUS 1.
	Will set all relevant flags at the same time.
*/
static unsigned char dec(unsigned char value)
{
	// Check for half carry
	if ((value & 0x0f) != 0)
	{
		FLAGS_CLEAR(FLAGS_HALFCARRY);
	}
	else
	{
		FLAGS_SET(FLAGS_HALFCARRY);
	}

	// Decrement value
	value--;

	// Check for zero
	if (value == 0)
	{
		FLAGS_SET(FLAGS_ZERO);
	}
	else
	{
		FLAGS_CLEAR(FLAGS_ZERO);
	}

	// Set negative
	FLAGS_SET(FLAGS_NEGATIVE);

	return value;
}

/*
	inc
	---
	Increment the given value by 1, performing all flag checks.
*/
static unsigned char inc(unsigned char value)
{
	// Check if all bits of lower byte are 1, which would cause a half carry.
	if ((value & 0x0f) == 0x0f)
	{
		FLAGS_SET(FLAGS_HALFCARRY);
	}
	else
	{
		FLAGS_CLEAR(FLAGS_HALFCARRY);
	}

	// Increment C
	value++;

	// Check if value is now 0.
	if (value == 0)
	{
		FLAGS_SET(FLAGS_ZERO);
	}
	else
	{
		FLAGS_CLEAR(FLAGS_ZERO);
	}

	// Always clear negative flag as this is an addition.
	FLAGS_CLEAR(FLAGS_NEGATIVE);

	return value;
}

/*
	and
	---
	Perform a bitwise AND function between the given value
	and register A.
	Will handle all flag setting at the same time.
*/

static void and (unsigned char value)
{
	registers.a &= value;

	// Check zero flag
	if (registers.a == 0)
	{
		FLAGS_SET(FLAGS_ZERO);
	}
	else
	{
		FLAGS_CLEAR(FLAGS_ZERO);
	}

	// Clear carry & negative flags
	FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE);

	// Set half-carry flag (as defined in OPCODE manual)
	FLAGS_SET(FLAGS_HALFCARRY);
}

/*
	xor
	---
	Perform a bitwise XOR function between register A and the given
	value.
*/
static void xor (unsigned char value) {
	registers.a ^= value;

	if (registers.a == 0)
	{
		FLAGS_SET(FLAGS_ZERO);
	}
	else
	{
		FLAGS_CLEAR(FLAGS_ZERO);
	}

	FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

/*
	or
	---
	Performs a bitwise OR function between register A and the given
	value.
*/
static void or(unsigned char value)
{
	registers.a |= value;

	if (registers.a == 0)
	{
		FLAGS_SET(FLAGS_ZERO);
	}
	else
	{
		FLAGS_CLEAR(FLAGS_ZERO);
	}

	FLAGS_CLEAR(FLAGS_CARRY | FLAGS_NEGATIVE | FLAGS_HALFCARRY);
}

/*
	cp
	---

*/
static void cp(unsigned char value)
{

}

/*===========================================
	INSTRUCTIONS
	------
	All instructions that can be executed.
============================================*/

/*
	0x0X
	INSTRUCTIONS
*/

/*
	NOP - 0x00
	---
	Does nothing. Skips the cycle.
*/
void nop(void)
{
}

/*
	LD BC NN - 0x01
	---
	Load into the 16-bit value BC the 16-bit value of NN.
*/
void ld_bc_nn(unsigned short value)
{
	registers.bc = value;
}

/*
	LD BCP A - 0x02
	---
	Load into the address specified by the 16-bit register BC, the value stored at the 8-bit register
	A.
*/
void ld_bcp_a(void)
{
	writeByte(registers.bc, registers.a);
}

/*
	DEC B - 0x05
	---
	Decrement the value held in register B.

	Must set flags:
		- Subtract
		- Zero (if applicable)
		- Half Carry (if applicable)
*/
void dec_b(void)
{
	registers.b = dec(registers.b);
}

/*
	LD B N - 0x06
	---
	Load value of N into register B.
*/
void ld_b_n(unsigned char value)
{
	registers.b = value;
}

/*
	DEC BC - 0x0B
	---
	Decrease the value of the 16-bit register BC by 1.
	Don't need to check flags (apparently?).
*/
void dec_bc(void)
{
	registers.bc--;
}

/*
	INC C - 0x0C
	---
	Increment C by 1.
	Clear negative flag.
	Set half-carry & zero flag if necessary.
*/
void inc_c(void)
{
	registers.c = inc(registers.c);
}

/*
	DEC C - 0x0D
	---
	Decrement the value held in register C.

	Must set flags:
		- Subtract
		- Zero (if applicable)
		- Half Carry (if applicable)
*/
void dec_c(void)
{
	registers.c = dec(registers.c);
}

/*
	LD C N - 0x0E
	---
	Load value of N into register C.
*/
void ld_c_n(unsigned char value)
{
	registers.c = value;
}

/*
	LD DE NN - 0x11
	---
	Load into the 16-bit register DE the value of NN.
*/
void ld_de_nn(unsigned short value)
{
	registers.de = value;
}

/*
	0x2X
	INSTRUCTIONS
*/

/*
	JR NZ N - 0x20
	---
	If ZERO FLAG is NOT set, add N to current PC.
	Effectively, jump forward or back by N if ZERO FLAG is NOT set.
*/
void jr_nz_n(unsigned char value)
{
	if (!FLAGS_ISZERO)
	{
		registers.pc += (signed char)value; // Jump based on a signed char, as a jump can be both
											// forward or backwards.
		ticks += 12;
	}
	else
	{
		ticks += 8;
	}
}

/*
	LD Hl NN - 0x21
	---
	Load the value of NN into the register HF
*/
void ld_hl_nn(unsigned short value)
{
	registers.hl = value;
}

/*
	LDI A (HL + ) - 0x2a
	---
	Load into the 8-bit register A, the value at the memory location specified by the value in the
	16-bit register HL.
	HL is incremeneted by 1 after this memory lead.
*/
void ldi_a_hlp(void)
{
	registers.a = readByte(registers.hl);
	registers.hl++;
}

/*
	LD SP NN - 0x31
	---
	Load 16-bit value NN into 16-bit register SP.
*/
void ld_sp_nn(unsigned short value)
{
	registers.sp = value;
}

/*
	LDD HL- A - 0x32
	---
	Load data from the 8-bit A register to the absolute address specified by the 16-bit register HL.
	The value of HL is decremented after the memory write.
*/
void ldd_hlp_a(void)
{
	writeByte(registers.hl, registers.a);
	registers.hl--;
}

/*
	LD HL N - 0x36
	---
	Load the 8-bit data N into the memory address speicfied by the 16-bit register HL.
*/
void ld_hlp_n(unsigned char value)
{
	writeByte(registers.hl, value);
}

/*
	LD A N - 0x3e
	---
	Load the 8-bit data N into the 8-bit register A.
*/
void ld_a_n(unsigned char value)
{
	registers.a = value;
}

/*
	0x4X
	INSTRUCTIONS
*/

/*
	LD B A - 0x47
	---
	Load the data from the 8-bit register A into the 8-bit register B.
*/
void ld_b_a(void)
{
	registers.b = registers.a;
}

/*
	0x7X
	INSTRUCTIONS
*/

/*
	LD A B 0x78
	---
	Load the the value of 8-bit register B into the 8-bit register A.
*/
void ld_a_b(void)
{
	registers.a = registers.b;
}

/*
	0xAX
	INSTRUCTIONS
*/

/*
	AND E - 0xA3
	---
	Perform bitwise AND between 8-bit registers A and E, storing the results in 8-bit register A.
*/
void and_e(void)
{
	and(registers.e);
}

/*
	XOR A - 0xAF
	---
	Calculates the result of an exclusive-or between the contents of register A
	and register A, storing the results back into register A.

	This should always result in clearing the contents of register A.
*/
void xor_a(void)
{
	xor(registers.a);
}

/*
	0xBX
	INSTRUCTIONS
*/

/*
	OR C - 0xB1
	---
	Perform a bitwise OR opperation between 8-bit registers A and C.
	Store the results in 8-bit register A.
	Check zero flag & clear all other flags
*/
void or_c(void)
{
	or(registers.c);
}

/*
	0xCX
	INSTRUCTIONS
*/

/*
	JP NN - 0xC3
	---
	Jumps to the point in code specified by the opperand.
*/
void jp_nn(unsigned short operand)
{
	// printf("Jumping to 0x%.4x\n", operand);
	registers.pc = operand;
}

/*
	RET - 0xC9
	---
	Generally used to return from a function.
	Read the last short value on the stack and assign it to the register.pc variable (program counter);
*/
void ret(void)
{
	registers.pc = readShortFromStack();
}

/*
	CALL NN - 0xCD
	---
	Set registers.pc (program counter) to 16-bit value NN.
	Store the original pc value on the stack before updating to new one!
*/
void call_nn(unsigned short value)
{
	writeShortToStack(registers.pc);
	registers.pc = value;
}

/*
	RST 18 - 0xDF
	---
	Restart / implied call function.

	Unconditional function call to the absolute fixed address defined by the opcode (0x0018).
	Will store the current PC on the stack before setting the PC to 0x0018.
	This is so that it can be returned to later if neccesary.
*/
void rst_18(void)
{
	writeShortToStack(registers.pc);
	registers.pc = 0x0018;
}

/*
	LD FF N AP - 0xE0
	---
	Load to the memory location 0xFFXX the value of the 8-bit register A.
	XX represents the 8-bit value N, which can have a value from 0x00 to 0xFF.
	Therefore, the range that can be loaded to is 0xFF00 - 0xFFFF.
*/
void ld_ff_n_ap(unsigned char value)
{
	writeByte((0xFF00 + value), registers.a);
}

/*
	LD FF C A - 0xE2
	---
	Load the data from the 8-bit register A into the memory location specified by 0xFF00 plus the 8-bit
	register C.
*/
void ld_ff_c_a(void)
{
	writeByte(0xFF + registers.c, registers.a);
}

/*
	LD NNP A - 0xEA
	---
	Load the data in the 8-bit register A to the memory location specified by NN.
*/
void ld_nnp_a(unsigned short value)
{
	writeByte(value, registers.a);
}

/*
	0xFX
	INSTRUCTIONS
*/

/*
	LD FF AP N - 0xF0
	---
	Load to the 8-bit register A the value stored at the 16-bit memory location defined as
	0xFF00 + N, with N being an 8-bit value.
*/
void ld_ff_ap_n(unsigned char value)
{
	registers.a = readByte(0xFF00 + value);
}

/*
	DI - 0xF3
	---
	Disable master interrupt flag.
*/
void di(void)
{
	interrupt.master = 0;
}

/*
	CP N - 0xFE
	---
	Updates flags based on the result of what A minus N would be.
	Does not update the A register.
*/
void cp_n(unsigned char value)
{
	// unsigned char result = registers.a - value;

	// // Is a subtraction so always set negative flag
	// FLAGS_SET(FLAGS_NEGATIVE);

	// // Set zero if result would be zero
	// if (result == 0)
	// {
	// 	FLAGS_SET(FLAGS_ZERO);
	// }
	// else
	// {
	// 	FLAGS_CLEAR(FLAGS_ZERO);
	// }

	// // If the value is larger than what's at register A, set carry flag
	// if (value > registers.a)
	// {
	// 	FLAGS_SET(FLAGS_CARRY);
	// }
	// else
	// {
	// 	FLAGS_CLEAR(FLAGS_CARRY);
	// }

	// // Same as above but just for the least sig bits
	// if ((value & 0x0f) > (registers.a & 0x0f))
	// {
	// 	FLAGS_SET(FLAGS_HALFCARRY);
	// }
	// else
	// {
	// 	FLAGS_CLEAR(FLAGS_HALFCARRY);
	// }
	FLAGS_SET(FLAGS_NEGATIVE);
	
	if(registers.a == value) FLAGS_SET(FLAGS_ZERO);
	else FLAGS_CLEAR(FLAGS_ZERO);
	
	if(value > registers.a) FLAGS_SET(FLAGS_CARRY);
	else FLAGS_CLEAR(FLAGS_CARRY);
	
	if((value & 0x0f) > (registers.a & 0x0f)) FLAGS_SET(FLAGS_HALFCARRY);
	else FLAGS_CLEAR(FLAGS_HALFCARRY);
}

/*
	RST 38 - 0xFF
	---
	Restart / implied call function.

	Unconditional function call to the absolute fixed address defined by the opcode (0x0038).
	Will store the current PC on the stack before setting the PC to 0x0038.
	This is so that it can be returned to later if neccesary.
*/

void rst_38(void)
{
	writeShortToStack(registers.pc);
	registers.pc = 0x0038;
}