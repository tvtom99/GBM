#include <stdio.h>
#include "../include/registers.h"

//code taken from Cinoop for debug purposes
void printRegisters(void) {
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
	// printf("IME: 0x%02x\n", interrupt.master);
	// printf("IE: 0x%02x\n", interrupt.enable);
	// printf("IF: 0x%02x\n", interrupt.flags);
}


//not currently used