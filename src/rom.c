#include "../include/rom.h"
#include "../include/memory.h"
#include "../include/main.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// Array of Strings. ROM types pulled from p11 './references/GBCPUman.pdf'
const char *romTypeString[256] = {
    [ROM_PLAIN] = "ROM_PLAIN",
    [ROM_MBC1] = "ROM_MBC1",
    [ROM_MBC1_RAM] = "ROM_MBC1",
    [ROM_MBC1_RAM_BATT] = "ROM_MBC1_RAM_BATT",
    [ROM_MBC2] = "ROM_MBC2",
    [ROM_MBC2_BATTERY] = "ROM_MBC2_BATTERY",
    [ROM_RAM] = "ROM_RAM",
    [ROM_RAM_BATTERY] = "ROM_RAM_BATTERY",
    [ROM_MMM01] = "ROM_MMM01",
    [ROM_MMM01_SRAM] = "ROM_MMM01_SRAM",
    [ROM_MMM01_SRAM_BATT] = "ROM_MMM01_SRAM_BATT",
    [ROM_MBC3_TIMER_BATT] = "ROM_MBC3_TIMER_BATT",
    [ROM_MBC3_TIMER_RAM_BATT] = "ROM_MBC3_TIMER_RAM_BATT",
    [ROM_MBC3] = "ROM_MBC3",
    [ROM_MBC3_RAM] = "ROM_MBC3_RAM",
    [ROM_MBC3_RAM_BATT] = "ROM_MBC3_RAM_BATT",
    [ROM_MBC5] = "ROM_MBC5",
    [ROM_MBC5_RAM] = "ROM_MBC5_RAM",
    [ROM_MBC5_RAM_BATT] = "ROM_MBC5_RAM_BATT",
    [ROM_MBC5_RUMBLE] = "ROM_MBC5_RUMBLE",
    [ROM_MBC5_RUMBLE_SRAM] = "ROM_MBC5_RUMBLE_SRAM",
    [ROM_MBC5_RUMBLE_SRAM_BATT] = "ROM_MBC5_RUMBLE_SRAM_BATT",
    [ROM_POCKET_CAMERA] = "ROM_POCKET_CAMERA",
    [ROM_BANDAI_TAMA5] = "ROM_BANDAI_TAMA5",
    [ROM_HUDSON_HUC3] = "ROM_HUDSON_HUC3",
    [ROM_HUDSON_HUC1] = "ROM_HUDSON_HUC1",
};

int loadROM(char *fileName)
{
    char name[17];
    enum romType type;
    int romSize;
    int ramSize;

    int i;

    FILE *f;
    size_t length;

    // Headers of ROM files are minimum size of ROM
    unsigned char header[0x180];

    // Open ROM file
    f = fopen(fileName, "rb");
    if (!f)
        return 0;

    // SIZE CHECK
    printf("Checking ROM size...\n");
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    if (length < 0x180)
    {
        printf("ROM is too small!\n");
        fclose(f);
        return 0;
    }
    printf("ROM size pass at %d bytes.\n", length);

    rewind(f);

    // Read the header of the rom into the 'header' variable.
    fread(header, 0x180, 1, f);

    // Init all values in the name array to '\0', NULL.
    memset(gameName, '\0', 17);

    // Loop through header and pull the name out of it.
    for (i = 0; i < 16; i++)
    {
        // If the value equals to 0x80 or 0xc0, set to null.
        if (header[i + ROM_OFFSET_NAME] == 0x80 || header[i + ROM_OFFSET_NAME] == 0xc0)
            gameName[i] = '\0';
        // Otherwise, set to the value in the memory.
        else
            gameName[i] = header[i + ROM_OFFSET_NAME];
    }

    printf("ROM name: %s\n", gameName);

    type = header[ROM_OFFSET_TYPE];

    // Ensure that type is not an invalid value
    if (type > 256) // 256 is the largest value type can be, as can be seen by declaring of 'romTypeString'
    {
        printf("Unknown ROM type: %#02x\n", type);
        fclose(f);
        return 0;
    }

    printf("ROM type: %s\n", romTypeString[type]);

    //BELOW IS SOME ROM AND RAM SIZE CHEKCING I DONT UNDERSTAND YET! IF THE .rom FILE USED IS 100% 
    //A PROPER ROM, THESE WILL ALWAYS PASS. WAIT ACTUALLY I THINK THESE ARE NOT NEEDED? 
    //MORE TESTING TO FOLLOW, MAYBE DELETE THIS?

    // romSize = header[ROM_OFFSET_ROM_SIZE];
    // if ((romSize & 0xF0) == 0x50)
    //     romSize = (int)pow(2.0, (double)(((0x52) & 0xF) + 1)) + 64;
    // else
    //     romSize = (int)pow(2.0, (double)(romSize + 1));

	
	// if(length != romSize * 16 * 1024) {
	// 	printf("ROM filesize does not equal ROM size!\n");
	// }

    // ramSize = header[ROM_OFFSET_RAM_SIZE];


    rewind(f);
    fread(cart, length, 1, f);

    fclose(f);
    return 1;
}