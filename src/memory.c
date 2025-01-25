/*
    NAME: TMC
    INIT DATE: 14/01/2024
    LAST EDIT DATE: 14/01/2024
    DESC:
        This is where the memory for the game is created, stored, and worked with.
*/

#include "../include/memory.h"
#include <stdio.h>
#include "../include/registers.h"
#include "../include/main.h"
#include "../include/keys.h"
#include "../include/interupts.h"
#include "../include/gpu.h"
#include <stdlib.h>

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

unsigned char *cart;        // The cart variable holds the information loaded in from the 'loadROM' method in rom.c
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
        Contents - Register for reading joy pad info.
        Bit represents 0b00000000
        Bits number ... 7654 3210;
            7 & 6 - no effect
            5 - Select buttons (if flagged, 0, 1, 2, 3 represent a, b, select, start )
            4 - Select dpad (if flagged, 0, 1, 2, 3 represent right, left, up, down )
            3 - start / down
            2 - select / up
            1 - b / left
            0 - a / right

        Bits are considered active when set to 0, NOT 1!

    */
    if (address == 0xFF00)
    {
        // // Check if button interupt is flagged
        // if (!(io[0x00] & 0x20))
        // {
        //     // return 1100 0000 | KEYS.KEYS1 | 0010 0000 = 1110 XXXX where X = KEYS.KEYS2 bits
        //     return (unsigned char)(0xC0 | keys.keys1 | 0x10);
        // }
        // // Check if dpad input flagged
        // else if (!(io[0x00] & 0x10))
        // {
        //     return (unsigned char)(0xc0 | keys.keys2 | 0x20);
        // }
        // // 0x30 = 0011 0000, i.e. if neither dpad and action button flags are set
        // else if (!(io[0x00] & 0x30))
        // {
        //     return 0xFF; // If neither bits are set, return 1111 1111
        // }
        // else
        // {
        //     return 0; // just incase?
        // }

        // for now, just return '0xff' cause nothing should be pressed!
        return (unsigned char)0xff;
    }

    /*
        Address @ Interrupt Flags
    */
    if (address == 0xFF0F)
    {
        return interrupt.flags;
    }

    /*
        Address @ Interrupt Enable
    */
    if (address == 0xFFFF)
    {

        return interrupt.enable;
    }

    // Address @ HRAM
    if (address >= 0xFF80 && address <= 0xFFFE)
    {
        return hram[address - 0xFF80];
    }

    // TAKEN FROM CINOOP CAUSE DIV TIMER IS VERY INVOLVED TO EMULATE PROPERLY
    // Should return a div timer, but a random number works just as well for Tetris
    if (address == 0xff04)
        return (unsigned char)rand();

    if (address == 0xff40)
        return gpu.control;
    if (address == 0xff42)
        return gpu.scrollY;
    if (address == 0xff43)
        return gpu.scrollX;
    if (address == 0xff44)
        return 0x90; // default to 0x90 until I got GPU working    
        // return gpu.scanline; // Read only. There is no equivalent in 'writeByte'.

    // Shouldn't get here! Attempting to read a memory address outside of valid ranges.
    printf("ERROR: Attempted to read invalid memory address: %x.\n", address);
    return 0;
}

void writeByte(unsigned short address, unsigned char value)
{
    // CINOOP TETRIS PATCH so I can get past the copyright screen maybe?
    // if (address == 0xFF80)
    // {
    //     printf("TETRIS PATCH\n");
    //     printf("TETRIS PATCH\n");
    //     printf("TETRIS PATCH\n");
    //     printf("TETRIS PATCH\n");
    //     printf("TETRIS PATCH\n");
    //     // quit();
    //     return;
    // }

    // Comments have been abreviated in this function. Please see 'readByte' to understand more of what's happening.

    // Address @ VRAM
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        vram[address - 0x8000] = value;
        if (address <= 0x97ff)
        {
            printf("Should run 'updateTile' here.\n");
            // updateTile(address, value);
        }
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
    // Individuals
    else if (address == 0xff40)
        gpu.control = value;
    else if (address == 0xff42)
        gpu.scrollY = value;
    else if (address == 0xff43)
        gpu.scrollX = value;
    else if (address == 0xff46)
        copy(0xfe00, value << 8, 160); // OAM DMA

    // Background and sprite palette
    else if (address == 0xff47)
    { // write only
        int i;
        // for(i = 0; i < 4; i++) backgroundPalette[i] = palette[(value >> (i * 2)) & 3];
        printf("Background palette being updated\n");
    }

    else if (address == 0xff48)
    { // write only
        int i;
        // for(i = 0; i < 4; i++) spritePalette[0][i] = palette[(value >> (i * 2)) & 3];
        printf("Sprite palette being updated\n");
    }

    else if (address == 0xff49)
    { // write only
        int i;
        // for(i = 0; i < 4; i++) spritePalette[1][i] = palette[(value >> (i * 2)) & 3];
        printf("Sprite palette being updated\n");
    }

    // Address @ Interrupt Flags
    else if (address == 0xFF0F)
    {
        interrupt.flags = value;
        io[address - 0xFF00] = value;
    }

    // Fallback
    else if (address >= 0xFF00 && address <= 0xFF7F)
    {
        io[address - 0xFF00] = value;
    }

    // Address @ HRAM
    else if (address >= 0xFF80 && address <= 0xFFFE)
    {
        hram[address - 0xFF80] = value;
    }

    // Address @ Interrupt Enable
    else if (address == 0xFFFF)
    {
        interrupt.enable = value;
    }

    // Shouldn't get here! Attempting to read a memory address outside of valid ranges.
    else
    {
        printf("ERROR: Attempted to write invalid memory address: 0x%02x.\n", address);
        printf("Generally I would quit here but I think I read that tetris does this sometimes for no reason...\n");
        // quit();
    }
}

unsigned short readShort(unsigned short address)
{
    unsigned short rShort = readByte(address) | (readByte(address + 1) << 8);
    // Returned short = XXXXXXXX YYYYYYYY, where X the binary representation of the first byte,
    // and Y is the binary representation of the second byte.
    return rShort;
}

/*
    writeShort
    ---
    Writes the given short value into bytes at address & address + 1.
    The least significant bytes of the short are put into the value at address.
    The most significant bytes are put into the value at address + 1.
*/
void writeShort(unsigned short address, unsigned short value)
{
    writeByte(address, (unsigned char)value & 0x00ff);
    writeByte(address + 1, (unsigned char)value & 0xff00 >> 8); // shifted to the right by 8 bits
}

/*
    writeShortToStack
    ---
    Writes the given value to the stack.
*/
void writeShortToStack(unsigned short value)
{
    // Move stack back by 2 points (stack counts down from a max value!)
    registers.sp -= 2;

    // Write the short
    writeShort(registers.sp, value);

    // DEBUG
    // printf("Stack write 0x%04x\n", value);
}

/*
    readShortFromStack
    ---
    reads the given value to the stack.
*/
unsigned short readShortFromStack(void)
{
    // read the short
    unsigned short value = readShort(registers.sp);

    // Move stack back by 2 points (stack counts down from a max value!)
    registers.sp += 2;

    // DEBUG
    // printf("Stack read 0x%04x\n", value);

    return value;
}

// Copy function taken from Cinoop cause I'm getting annoyed that my emulator is looping and I don't have ths written already
void copy(unsigned short destination, unsigned short source, size_t length)
{
    unsigned int i;
    for (i = 0; i < length; i++)
        writeByte(destination + i, readByte(source + i));
}