
#include <stdio.h>
#include "../include/interupts.h"
#include "../include/display.h"
#include "../include/registers.h"
#include "../include/memory.h"
#include "../include/cpu.h"

// for debug include keys.h
#include "../include/keys.h"

struct interrupt interrupt;

void interruptStep(void)
{
    // printf("Running interupt step!\n");

    // printf("interrupt master: 0x%.02x\ninterrupt enable: 0x%.02x\ninterrupt flags: 0x%.02x\n", interrupt.master, interrupt.enable, interrupt.flags);
    // printf("Keys currently: 0x%.04x\n", keys.c);

    // If the master & enable flags are set, and any spcific interupts are on...
    if (interrupt.master && interrupt.enable && interrupt.flags)
    {
        // Find and execute the correct interrupt.
        printf("Running an interupt!");

        //'activate' will have a positive bit for whichever flags are BOTH set AND enabled during this step
        unsigned char activate = (interrupt.enable & interrupt.flags);

        /*
            T H E   O R D E R   O F   T H E   I N T E R R U P T S   I N   T H E   ' I F '
            S T A T E M E N T   R E F L E C T S   T H E I R   R E L A T I V E
            P R I O R I T I E S   W H E N   B E I N G   E X E C U T E D
        */

        // If VLBANK is set AND it has been allowed...
        if (activate & INTERRUPTS_VBLANK)
        {
            printf("Running VBLANK interrupt.");
            interrupt.flags = (interrupt.flags & ~INTERRUPTS_VBLANK); // switch off the flag...
            vblank();
        }

        // If STAT is set AND it has been allowed...    0x48
        if (activate & INTERRUPTS_LCDSTAT)
        {
            printf("Running LCDSTAT interrupt.");
            interrupt.flags = (interrupt.flags & ~INTERRUPTS_LCDSTAT);
            STATInterrupt();
        }

        // If TIMER is set AND it has been allowed...   0x50
        if (activate & INTERRUPTS_TIMER)
        {
            printf("Running TIMER interrupt.");
            interrupt.flags = (interrupt.flags & ~INTERRUPTS_TIMER);
            timer();
        }

        // If SERIAL is set AND it has been allowed...  0x58
        if (activate & INTERRUPTS_SERIAL)
        {
            printf("Running SERIAL interrupt.");
            interrupt.flags = (interrupt.flags & ~INTERRUPTS_SERIAL);
            serial();
        }

        // If JOYPAD is set AND it has been allowed...  0x60
        if (activate & INTERRUPTS_JOYPAD)
        {
            printf("Running JOYPAD interrupt.");
            interrupt.flags = (interrupt.flags & ~INTERRUPTS_JOYPAD);
            joypad();
        }
    }
}

/*
    vblank
    ---
    This function draws to the screen. It executes ~60 times a second.
*/
void vblank(void)
{
    printf("vblank interrupt running\n");

    // Draw the frame for this section
    drawFramebuffer();

    // Reset the master interupt flag
    interrupt.master = 0;

    // As per docs...
    /*
        The corresponding interrupt handler (see the IE and IF register descriptions above) is
        called by the CPU. This is a regular call, exactly like what would be performed by a
        call <address> instruction (the current PC is pushed onto the stack and then set to
        the address of the interrupt handler).
    */
    // vblank interrupt handler is at memory location is 0x40, so that is where registers.pc
    // will be set to.
    writeShortToStack(registers.pc);
    registers.pc = 0x40;

    // Increment the ticks this would take
    ticks += 12;
}

/*
    STATInterrupt
    ---
    STATInterrupt is the interrupt handler for when a STAT interrupt is called.
    The STAT interrupt is handled at memory location 0x48.
*/
void STATInterrupt(void)
{
    printf("STAT interrupt running\n");

    // Reset master interupt flag
    interrupt.master = 0;

    // Save pc to stack and jump to execute from location 0x48
    writeShortToStack(registers.pc);
    registers.pc = 0x48;

    // Increment the ticks this would take
    ticks += 12;
}

/*
    timer
    ---
    timer is the interrupt handler for when a timer interrupt is called.
    The timer interrupt is handled at memory location 0x50.
*/
void timer(void)
{
    printf("timer interrupt running\n");

    // Reset master interupt flag
    interrupt.master = 0;

    // Save pc to stack and jump to execute from location 0x48
    writeShortToStack(registers.pc);
    registers.pc = 0x50;

    // Increment the ticks this would take
    ticks += 12;
}

/*
    serial
    ---
    serial is the interrupt handler for when a serial interrupt is called.
    The serial interrupt is handled at memory location 0x50.
*/
void serial(void)
{
    printf("serial interrupt running\n");

    // Reset master interupt flag
    interrupt.master = 0;

    // Save pc to stack and jump to execute from location 0x58
    writeShortToStack(registers.pc);
    registers.pc = 0x58;

    // Increment the ticks this would take
    ticks += 12;
}

/*
    joypad
    ---
    joypad is the interrupt handler for when a joypad interrupt is called.
    The joypad interrupt is handled at memory location 0x50.
*/
void joypad(void)
{
    printf("joypad interrupt running\n");

    // Reset master interupt flag
    interrupt.master = 0;

    // Save pc to stack and jump to execute from location 0x60
    writeShortToStack(registers.pc);
    registers.pc = 0x60;

    // Increment the ticks this would take
    ticks += 12;
}

/*
    returnFromInterrupt
    ---
    This is taken directly from Cinoop, but it is effectively the simplest way to write;
    "enable interrupts again then jump back to whatever value as at the top of the stack"
*/
void returnFromInterrupt(void)
{
    interrupt.master = 1;
    registers.pc = readShortFromStack();
}