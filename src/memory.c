/*
    NAME: TMC
    INIT DATE: 14/01/2024
    LAST EDIT DATE: 14/01/2024
    DESC:
        This is where the memory for the game is created, stored, and worked with.
*/

#include "../include/memory.h"
#include <stdio.h>

// A variable that resets the IO to some necessary value when starting or reseting the system.
const unsigned char ioReset[0x100] = {
    0x0F, 0x00, 0x7C, 0xFF, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,
    0x80, 0xBF, 0xF3, 0xFF, 0xBF, 0xFF, 0x3F, 0x00, 0xFF, 0xBF, 0x7F, 0xFF, 0x9F, 0xFF, 0xBF, 0xFF,
    0xFF, 0x00, 0x00, 0xBF, 0x77, 0xF3, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
    0x91, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0xFF, 0xFE,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xC1, 0x00, 0xFE, 0xFF, 0xFF, 0xFF,
    0xF8, 0xFF, 0x00, 0x00, 0x00, 0x8F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
    0x45, 0xEC, 0x52, 0xFA, 0x08, 0xB7, 0x07, 0x5D, 0x01, 0xFD, 0xC0, 0xFF, 0x08, 0xFC, 0x00, 0xE5,
    0x0B, 0xF8, 0xC2, 0xCE, 0xF4, 0xF9, 0x0F, 0x7F, 0x45, 0x6D, 0x3D, 0xFE, 0x46, 0x97, 0x33, 0x5E,
    0x08, 0xEF, 0xF1, 0xFF, 0x86, 0x83, 0x24, 0x74, 0x12, 0xFC, 0x00, 0x9F, 0xB4, 0xB7, 0x06, 0xD5,
    0xD0, 0x7A, 0x00, 0x9E, 0x04, 0x5F, 0x41, 0x2F, 0x1D, 0x77, 0x36, 0x75, 0x81, 0xAA, 0x70, 0x3A,
    0x98, 0xD1, 0x71, 0x02, 0x4D, 0x01, 0xC1, 0xFF, 0x0D, 0x00, 0xD3, 0x05, 0xF9, 0x00, 0x0B, 0x00};

unsigned char cart[0x8000]; // The cart variable holds the information loaded in from the 'loadROM' method in rom.c
unsigned char sram[0x2000]; // Switchable RAM
unsigned char io[0x100];    // Input - Output
unsigned char vram[0x2000]; // Video RAM
unsigned char oam[0x100];   // Sprite Attribute Memory (OAM)
unsigned char wram[0x2000]; // Working RAM, Internal RAM
unsigned char hram[0x80];   // Internal RAM, High RAM. The ram actually in the CPU die, where the wram is seperate.

/*
        MEMORY MAP
            Interrupt Enable Register
            --------------------------- FFFF
            Internal RAM
            --------------------------- FF80
            Empty but unusable for I/O
            --------------------------- FF4C
            I/O ports
            --------------------------- FF00
            Empty but unusable for I/O
            --------------------------- FEA0
            Sprite Attrib Memory (OAM)
            --------------------------- FE00
            Echo of 8kB Internal RAM
            --------------------------- E000
            8kB Internal RAM
            --------------------------- C000
            8kB switchable RAM bank
            --------------------------- A000
            8kB Video RAM
            --------------------------- 8000 --
            16kB switchable ROM bank |
            --------------------------- 4000 |= 32kB Cartrigbe
            16kB ROM bank #0 |
            --------------------------- 0000 --
            * NOTE: b = bit, B = byte
    */

unsigned char readByte(unsigned short address)
{
    // Address @ Cart
    if (address <= 0x7FFF)
    {
        return cart[address];
    }

    // Address @ VRAM
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        return vram[address - 0x8000]; // The vram array is 0x2000 in size but the address will be
                                       // anywhere from 0x8000 to 0x9FFF. MINUS 0x8000 to bring
                                       // address in range.
    }

    // Adress @ SRAM
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        return sram[address - 0xA000]; // Same reasoning as above
    }

    // Adress @ WRAM
    if (address >= 0xC000 && address <= 0xDFFF)
    {
        return wram[address - 0xC000]; // Same reasoning as above
    }

    // Address @ WRAM (echo)
    /*
        Explanation of what is happening with echo RAM;
            "When a 16 bin memory bus for 64k is used with only 32k memory.
            ...Or, we could be cheap and not wire up the 16th address wire to anything. What happens is
            that the upper bit of the address is actually ignored, and the memory is aliased into both
            halves of the address space. If you write to $0200, you're also changing $8200, and vice
            versa." - https://www.reddit.com/r/EmuDev/comments/ogvjby/confusion_on_gameboy_rom_banking/

        Basically, memory values within this range should return the same value as if they were within
        the above check's range. This is because the most significant bit is ignored in WRAM.
        E.g. 0xC123 is read the same as 0xE123.
    */
    if (address >= 0xE000 && address <= 0xFDFF)
    {
        return wram[address - 0xE000];
    }

    // Address @ OAM
    /*
        I'm including the 'empty but unusable for I/O' bits of memory here cause Cinoop did it. Not sure why though?
        I think I'll find out when I begin learning more about displaying graphics, as that is what this
        memory is handling.
    */
    if (address >= 0xFE00 && address <= 0xFEFF)
    {
        return oam[address - 0xFE00];
    }

    // Address @ IO
    /*
        These are a bit different, and I will label each one with what it's used for as per GB Manual.
    */

    /*
        Name - P1
        Contents - Register for reading joy pad info
                   and determining system type. (R/W)
    */
    // if (address == 0xFF00)
    // {

    // }
    // I'm going to ignore these special cases for now cause they rely on user input (which I don't have).
    // I want my emulator starting and running up to the first point a user will provide input before I
    // implement this stuff.
    if (address >= 0xFF00 && address <= 0xFF79)
    {
        return io[address - 0xFF00];
    }

    // Address @ HRAM
    if (address >= 0xFF80 && address <= 0xFFFE)
    {
        return hram[address - 0xFF80];
    }

    // Shouldn't get here! Attempting to read a memory address outside of valid ranges.
    printf("ERROR: Attempted to read invalid memory address: %x.\n", address);
    return 0;
}

void writeByte(unsigned short address, unsigned char value)
{
    // Comments have been abreviated in this function. Please see 'readByte' to understand more of what's happening.

    // Address @ VRAM
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        vram[address - 0x8000] = value;
    }

    // Adress @ SRAM
    else if (address >= 0xA000 && address <= 0xBFFF)
    {
        sram[address - 0xA000] = value;
    }

    // Adress @ WRAM
    else if (address >= 0xC000 && address <= 0xDFFF)
    {
        wram[address - 0xC000] = value;
    }

    // Address @ WRAM (echo)
    else if (address >= 0xE000 && address <= 0xFDFF)
    {
        wram[address - 0xE000] = value;
    }

    // Address @ OAM
    else if (address >= 0xFE00 && address <= 0xFEFF)
    {
        oam[address - 0xFE00] = value;
    }

    // Address @ IO
    else if (address >= 0xFF00 && address <= 0xFF79)
    {
        io[address - 0xFF00] = value;
    }

    // Address @ HRAM
    else if (address >= 0xFF80 && address <= 0xFFFE)
    {
        hram[address - 0xFF80] = value;
    }

    //DEBUG FOR NOW JUST SO THAT GAME TRIES TO START
    else if(address == 0xFFFF)
    {
        printf("Trying to write to interupts. Not implemented...\n");
    }

    // Shouldn't get here! Attempting to read a memory address outside of valid ranges.
    else
    {
        printf("ERROR: Attempted to write invalid memory address: %x.\n", address);
    }
}

unsigned short readShort(unsigned short address)
{
    // Returned short = XXYY, where XX is from the first address, and YY is ffrom the next value along.
    return readByte(address) | (readByte(address + 1) << 8);
}