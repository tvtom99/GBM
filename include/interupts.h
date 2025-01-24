#pragma once

// Interupts definitions taken from Cinoop
#define INTERRUPTS_VBLANK (1 << 0)  // First bit is VBLANK interupt
#define INTERRUPTS_LCDSTAT (1 << 1) // Second bit is LCDSTAT
#define INTERRUPTS_TIMER (1 << 2)   // Third bit is TIMER interupt
#define INTERRUPTS_SERIAL (1 << 3)  // Fourth bit is SERIAL interupt
#define INTERRUPTS_JOYPAD (1 << 4)  // Fifth bit is JOYPAD interupt

struct interrupt
{
    unsigned char master;
    unsigned char enable;
    unsigned char flags;
} extern interrupt;


void interruptStep(void);
void vblank(void);
void STATInterrupt(void);
void timer(void);
void serial(void);
void joypad(void);