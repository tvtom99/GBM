/*
    NAME: TMC
    INIT DATE: 14/01/2024
    LAST EDIT DATE: 14/01/2024
    DESC:
        Header for the rom file. Contains information on how ROM files for GameBoy are constructed.
*/

#pragma once

//Offests into memory that define certain parts of the game.
#define ROM_OFFSET_NAME 0x134   //What the ROM says is its name.
#define ROM_OFFSET_TYPE 0x147   //What type of ROM it is. Preset, see lower enum.
#define ROM_OFFSET_ROM_SIZE 0x148   //What size is the ROM.
#define ROM_OFFSET_RAM_SIZE 0x149   //How much RAM does the ROM have.

//Enum of all ROM types. Taken from Cinoop, but information found at p11: http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf (also in '/references')
enum romType {
	ROM_PLAIN = 0x00,
	ROM_MBC1 = 0x01,
	ROM_MBC1_RAM = 0x02,
	ROM_MBC1_RAM_BATT = 0x03,
	ROM_MBC2 = 0x05,
	ROM_MBC2_BATTERY = 0x06,
	ROM_RAM = 0x08,
	ROM_RAM_BATTERY = 0x09,
	ROM_MMM01 = 0x0B,
	ROM_MMM01_SRAM = 0x0C,
	ROM_MMM01_SRAM_BATT = 0x0D,
	ROM_MBC3_TIMER_BATT = 0x0F,
	ROM_MBC3_TIMER_RAM_BATT = 0x10,
	ROM_MBC3 = 0x11,
	ROM_MBC3_RAM = 0x12,
	ROM_MBC3_RAM_BATT = 0x13,
	ROM_MBC5 = 0x19,
	ROM_MBC5_RAM = 0x1A,
	ROM_MBC5_RAM_BATT = 0x1B,
	ROM_MBC5_RUMBLE = 0x1C,
	ROM_MBC5_RUMBLE_SRAM = 0x1D,
	ROM_MBC5_RUMBLE_SRAM_BATT = 0x1E,
	ROM_POCKET_CAMERA = 0x1F,
	ROM_BANDAI_TAMA5 = 0xFD,
	ROM_HUDSON_HUC3 = 0xFE,
	ROM_HUDSON_HUC1 = 0xFF,
};

extern const char *romTypeString[256];

int loadROM(char *filename);