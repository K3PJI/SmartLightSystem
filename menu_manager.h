#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include <Arduino.h>
#include "config.h"

void Menu_Manager_Update();

void User_Activity_Detected();

void Save_And_Apply_Settings();

extern const char *MAIN_MENU_ITEMS[];
extern const char *LIGHT_SETTINGS_ITEMS[];
extern const char *CEILING_MODES_ITEMS[];
extern const char *STATIC_COLOR_ITEMS[];
extern const char *COLOR_PICKER_ITEMS[];
extern const char *SETTINGS_MENU_ITEMS[];
extern const char *CALIBRATION_ITEMS[];

#endif