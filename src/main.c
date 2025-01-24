/*
    NAME: TMC
    INIT DATE: 14/01/2024
    LAST UPDATE: 14/01/2024
    DESC:
        Initialise the emulator, read the input, and start the CPU cycle task.
*/

#include "../include/rom.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <SDL2/SDL.h>
#include "../include/cpu.h"
#include "../include/memory.h" //can remove this after debug
#include "../include/main.h"
#include "../include/interupts.h"
#include "../include/gpu.h"

char gameName[17];
unsigned char debugModeEnable = 1;

// int WinMain(int argc, char *argv[])
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Parse the command line into argc and argv (have to do this now the application is not just on console)
    int argc = 0;
    char *argv[256]; // Assuming no more than 255 arguments
    char *token = strtok(lpCmdLine, " ");

    while (token != NULL && argc < 256)
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    // Access argc and argv like in a console program
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    // Fail if no path to ROM file is provided
    if (argc < 1)
    {
        printf("Usage: ./<emulator_name> <path_to_rom>\n", argv[0]);
    }
    else
    {
        // Just assume the filename given is a ROM and get it loading!
        char *filename = argv[0];

        printf("Loading file \"%s\"...\n", filename);

        if (loadROM(filename) != 1)
        {
            printf("Failed rom load!\n");

            return 1;
        }

        // Rom has loaded properly, open window and start CPU cycle
        SDL_Init(SDL_INIT_VIDEO);

        char title[200];    // Buffer to hold window title

        // Combine "GBM - " with gameName
        strcpy(title, "GBM - "); // Copy "GBM - " to title
        strcat(title, gameName); // Append gameName to title

        //The window itself! Size defined by constants and scaling factor.
        SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_HEIGHT * SCALING_FACTOR, SCREEN_WIDTH * SCALING_FACTOR, SDL_WINDOW_SHOWN);

        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        SDL_Event e;
        int quit = 0;
        reset(); // Initialise all values needed to start the system.
        while (!quit)
        {
            stepCPU();
            stepGPU();
            interruptStep();
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    quit = 1;
                }
            }
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    return 0;
}

void quit(void)
{
    printf("Quiting emulator...\n");
    unloadROM();
    exit(1);
}