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
    if (argc < 2)
    {
        printf("Usage: ./<emulator_name> <path_to_rom>\n", argv[0]);
    }
    else
    {
        // Just assume the filename given is a ROM and get it loading!
        char *filename = argv[1];

        printf("Loading file \"%s\"...\n", filename);

        if (loadROM(filename) != 1)
        {
            printf("Failed!\n");

            return 1;
        }

        // Rom has loaded properly, start CPU cycle

        // Your SDL initialization and main loop here
        SDL_Init(SDL_INIT_VIDEO);
        SDL_Window *window = SDL_CreateWindow("SDL2 Application", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        SDL_Event e;
        int quit = 0;
        while (!quit)
        {
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

        //RE-ENABLE AND IMPLEMENT SDL WINDOW STUFF HERE

        // reset(); // Initialise all values needed to start the system.
        // while (1)
        // {
        //     stepCPU();
        // }
    }

    return 0;
}

void quit(void)
{
    printf("Quiting emulator...\n");
    exit(1);
}