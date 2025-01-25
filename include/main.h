#pragma once

#define SCREEN_HEIGHT 160
#define SCREEN_WIDTH 144
#define SCALING_FACTOR 3

extern char gameName[17];
extern unsigned char debugModeEnable;

void quit(void);
void handlePress(const char *key);
void handleUnpress(const char *key);