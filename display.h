#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

void Display_Init();
void Display_UpdateScreen();

void Display_Sleep();
void Display_Wake();
void Display_IsSleeping();

void Display_SetNeedsUpdate(bool needsUpdate);

void Display_SetContrast(uint8_t contrastPercent);

#endif