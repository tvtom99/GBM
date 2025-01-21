
#include "../include/interupts.h"
#include "../include/display.h"
#include "../include/registers.h"
#include "../include/memory.h"

struct interrupt interrupt;

void interruptStep(void)
{
    // If the master & enable flags are set, and any spcific interupts are on...
    if (interrupt.master && interrupt.enable && interrupt.flags)
    {
        // Find and execute the correct interrupt.

        //'activate' will have a positive bit for whichever flags are BOTH set AND enabled during this step
        unsigned char activate = (interrupt.enable & interrupt.flags);

        // If VLBANK is set AND it has been allowed...
        if (activate & INTERRUPTS_VBLANK)
        {
            interrupt.flags = (interrupt.flags & ~INTERRUPTS_VBLANK); // switch off the flag...
            vblank();
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
    // Draw the frame for this section
    drawFrame();

    // Reset the master interupt flag
    interrupt.master = 0;

    // As per docs... 
    /*
        The corresponding interrupt handler (see the IE and IF register descriptions above) is 
        called by the CPU. This is a regular call, exactly like what would be performed by a 
        call <address> instruction (the current PC is pushed onto the stack and then set to 
        the address of the interrupt handler).
    */
   //vblank memory location is 0x40, so that is where registers.pc will be set to.
    writeShortToStack(registers.pc);
    registers.pc = 0x40;

    //Increment the ticks this would take (I haven't implemented this yet...)
}