#include "../include/gpu.h"
#include "../include/cpu.h"
#include "../include/interupts.h"
#include "../include/main.h"
#include <stdio.h>

struct gpu gpu;

unsigned char tiles[384][8][8];

void stepGPU(void)
{
    enum gpuMode
    {
        GPU_MODE_HBLANK = 0,
        GPU_MODE_VBLANK = 1,
        GPU_MODE_OAM = 2,
        GPU_MODE_VRAM = 3,
    } static gpuMode = GPU_MODE_HBLANK;

    // This variable will always update to be what the CPU ticks were last time this function was called
    static int lastTicks = 0;

    // Update GPU tick to be the difference between the current CPU ticks & the CPU ticks last this was called
    gpu.tick += ticks - lastTicks;

    // Update lastTicks with CPU ticks
    lastTicks = ticks;

    // Based on which CPU mode, execute...
    switch (gpuMode)
    {
    case GPU_MODE_HBLANK:
        if (gpu.tick >= 204)
        {
            hblank();

            if (gpu.scanline == 143)
            {
                if (interrupt.enable & INTERRUPTS_VBLANK)
                    interrupt.flags |= INTERRUPTS_VBLANK;

                gpuMode = GPU_MODE_VBLANK;
            }

            else
                gpuMode = GPU_MODE_OAM;

            gpu.tick -= 204;
        }
        break;
    case GPU_MODE_VBLANK:
        if (gpu.tick >= 456)
        {
            gpu.scanline++;

            if (gpu.scanline > 153)
            {
                gpu.scanline = 0;
                gpuMode = GPU_MODE_OAM;
            }

            gpu.tick -= 456;
        }

        break;

    case GPU_MODE_OAM:
        if (gpu.tick >= 80)
        {
            gpuMode = GPU_MODE_VRAM;

            gpu.tick -= 80;
        }

        break;

    case GPU_MODE_VRAM:
        if (gpu.tick >= 172)
        {
            gpuMode = GPU_MODE_HBLANK;

            // printf("renderScanline() would be called here\n");
            // renderScanline();

            gpu.tick -= 172;
        }

        break;
    }
}

void hblank(void)
{
    gpu.scanline++;

    // if (gpu.scanline == 0x3D)
    // {
    //     printf("scanline is where it should be at the end of the loops\n");
    //     debugModeEnable = 1;
    // }
}