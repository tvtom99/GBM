#include <stdio.h>
#include "../include/registers.h"
#include "../include/main.h"
#include <SDL2/SDL.h>
#include "../include/cpu.h"
#include "../include/memory.h"
#include "../include/interupts.h"
#include "../include/keys.h"
#include "../include/gpu.h"

// code taken from Cinoop for debug purposes
void printRegisters(void)
{
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

void showRealtimeData(void)
{
	int buttonId;
	char debugMessage[5000];
	char *debugMessageP = debugMessage;
	char temp[1024] = "";

	// Get the last executed instruction (taken straight from cinoop)
	unsigned char instruction = readByte(registers.pc);
	unsigned short operand = 0;

	if (instructions[instruction].operandLength == 1)
		operand = (unsigned short)readByte(registers.pc + 1);
	if (instructions[instruction].operandLength == 2)
		operand = readShort(registers.pc + 1);

	debugMessageP += sprintf(debugMessageP, "OPCODE:\n");
	if (instructions[instruction].operandLength)
		debugMessageP += sprintf(debugMessageP, instructions[instruction].disassembly, operand);
	else
		debugMessageP += sprintf(debugMessageP, instructions[instruction].disassembly);
	debugMessageP += sprintf(debugMessageP, "\n\n");

	// Construct the debug message (I had my own code here but I wrote it with strcat and a temp
	// string val so I just copied Cinoop's code cause it works better and quicker than re-writing
	// mine)

	debugMessageP += sprintf(debugMessageP, "\nRegisters:\n");
	debugMessageP += sprintf(debugMessageP, "AF: 0x%04x\n", registers.af);
	debugMessageP += sprintf(debugMessageP, "BC: 0x%04x\n", registers.bc);
	debugMessageP += sprintf(debugMessageP, "DE: 0x%04x\n", registers.de);
	debugMessageP += sprintf(debugMessageP, "HL: 0x%04x\n", registers.hl);
	debugMessageP += sprintf(debugMessageP, "SP: 0x%04x\n", registers.sp);
	debugMessageP += sprintf(debugMessageP, "PC: 0x%04x\n", registers.pc);

	debugMessageP += sprintf(debugMessageP, "\nIME: 0x%02x\n", interrupt.master);
	debugMessageP += sprintf(debugMessageP, "IE: 0x%02x\n", interrupt.enable);
	debugMessageP += sprintf(debugMessageP, "IF: 0x%02x\n", interrupt.flags);

	debugMessageP += sprintf(debugMessageP, "\nKEYS: 0x%02x\n", keys.c);
	debugMessageP += sprintf(debugMessageP, "\nKEYS MEM: 0x%02x\n", readByte(0xFF00));

	debugMessageP += sprintf(debugMessageP, "\nGPU control (0xFF40): 0x%02x\n", gpu.control);
	debugMessageP += sprintf(debugMessageP, "GPU scrollX (0xFF43): 0x%02x\n", gpu.scrollX);
	debugMessageP += sprintf(debugMessageP, "GPU scrollY (0xFF42): 0x%02x\n", gpu.scrollY);
	debugMessageP += sprintf(debugMessageP, "GPU Scanline (0xFF44): 0x%02x\n", gpu.scanline);
	debugMessageP += sprintf(debugMessageP, "GPU tick: 0x%02x\n", gpu.tick);

	debugMessageP += sprintf(debugMessageP, "\nContinue debugging?\n");

	SDL_MessageBoxButtonData buttons[] = {
		{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK"},
		{SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No"},
		{SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Quit"}

	};

	SDL_MessageBoxData boxData = {
		SDL_MESSAGEBOX_INFORMATION,
		NULL,
		"Debug Mode",
		debugMessage,
		SDL_arraysize(buttons),
		buttons,
		NULL};

	SDL_ShowMessageBox(&boxData, &buttonId);

	if (buttonId != 0)
	{
		if (buttonId == (1))
		{
			debugModeEnable = 0;
			printf("Stopping debug.\n");
		}
		else
		{
			printf("Stopping debug.\n");
			quit();
		}
	}
}
// not currently used