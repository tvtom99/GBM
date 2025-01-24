#pragma once

struct gpu {
	unsigned char control;
	unsigned char scrollX;
	unsigned char scrollY;
	unsigned char scanline;
	unsigned long tick;
} extern gpu;

void
stepGPU(void);
void hblank(void);