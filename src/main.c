/*
    NAME: TMC
    INIT DATE: 14/01/2024
    LAST UPDATE: 14/01/2024
    DESC:
        Initialise the emulator, read the input, and start the CPU cycle task.
*/

#include "../include/rom.h"
#include <stdio.h>
#include "../include/cpu.h"
#include "../include/memory.h"  //can remove this after debug
#include "include/main.h"

int main(int argc, char *argv[])
{
    //Fail if no path to ROM file is provided
    if (argc < 2)
    {
        printf("Usage: ./<emulator_name> <path_to_rom>\n", argv[0]);
    }
    else
    {
        //Just assume the filename given is a ROM and get it loading!
        char* filename = argv[1];

        printf("Loading file \"%s\"...\n", filename);

        if (loadROM(filename) != 1)
        {
            printf("Failed!\n");

            return 1;
        }

        //Rom has loaded properly, start CPU cycle
        while(1)
        {
            reset();
            stepCPU();
        }

    }

    return 0;
}

void quit(void) {
	exit(1);
}