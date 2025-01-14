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

int main(int argc, char *argv[])
{
    // DEBUGGING (while learning C)
    printf("argc = %d\n", argc);

    if (argc < 2)
    {
        printf("Usage: ./<emulator_name> <path_to_rom>\n", argv[0]);
    }
    else
    {
        printf("argv = %s\n", argv[1]);

        char* filename = argv[1];

        printf("Loading file \"%s\"...\n", filename);

        if (!loadROM(filename))
        {
            printf("Failed!\n");

            return 1;
        }
    }

    return 0;
}