/*
    NAME: TMC
    INITAL DATE: 14/01/2024
    LAST EDIT DATE: 16/01/2024
    DESC:
        Describe the memory structures in the console
*/

#pragma once

#include <stddef.h>

extern const unsigned char ioReset[0x100];

extern unsigned char *cart;
extern unsigned char sram[0x2000];
extern unsigned char io[0x100];
extern unsigned char vram[0x2000];
extern unsigned char oam[0x100];
extern unsigned char wram[0x2000];
extern unsigned char hram[0x80];

unsigned char readByte(unsigned short address);
void writeByte(unsigned short address, unsigned char value);

unsigned short readShort(unsigned short address);
void writeShortToStack(unsigned short value);
unsigned short readShortFromStack(void);
void copy(unsigned short destination, unsigned short source, size_t length);