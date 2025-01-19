/*
    NAME: TMC
    INITAL DATE: 14/01/2024
    LAST EDIT DATE: 16/01/2024
    DESC:
        Describe the memory structures in the console
*/

extern const unsigned char ioReset[0x100];

extern unsigned char cart[0x8000];
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