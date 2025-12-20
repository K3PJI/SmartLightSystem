#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <Arduino.h>

void Led_Init();
void Led_Update();

void Led_SetCeilingBrightness(uint8_t brightness);
void Led_SetMonitorBrightness(uint8_t brightness);
void Led_SetCeilingHSV(uint16_t h, uint8_t s, uint8_t v);
void Led_SetCeilingColorPreset(int preset);
void Led_ToggleMonitorColor();

#endif