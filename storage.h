#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include "config.h"

void Storage_Init();

struct Settings;

void Storage_SaveSettings(const Settings *settings);

bool Storage_LoadSettings(Settings *settings);

void Storage_Reset();

#endif