/*
    NAME: TMC
    INIT DATE: 14/01/2024
    LAST EDIT DATE: 14/01/2024
    DESC:
        This header file contains the definitions for all the memory registers used by the Gameboy.
        It uses anonymous structs and unions so that I can refer to registers seperately (as their 8-bit representation)
        or as combined 16-bit variations.
        Eg. 
            registers.b = 0x56;    // Set the upper byte of `bc`
            registers.c = 0x78;    // Set the lower byte of `bc`
            printf("%x\n", registers.bc); // Prints "5678"
            
        The Gameboy CPU has 8 registers, 1 of which is used to store flags for when operations cause certain conditions.
        (zero flag, negative flag, half carry flag, and full carry flag)
        Each register is 8-bits, but they can be accessed as a combined 16-bit register too.

    This code was initially copied from the open-source emulator Cinoop (https://cturt.github.io/cinoop.html).
*/

#pragma once

struct registers {
	struct {
		union {
			struct {
				unsigned char f;
				unsigned char a;
			};
			unsigned short af;
		};
	};
	
	struct {
		union {
			struct {
				unsigned char c;
				unsigned char b;
			};
			unsigned short bc;
		};
	};
	
	struct {
		union {
			struct {
				unsigned char e;
				unsigned char d;
			};
			unsigned short de;
		};
	};
	
	struct {
		union {
			struct {
				unsigned char l;
				unsigned char h;
			};
			unsigned short hl;
		};
	};
	
	unsigned short sp;
	unsigned short pc;
} extern registers;