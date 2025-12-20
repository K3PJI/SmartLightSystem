#include "storage.h"
#include <EEPROM.h>

#define STORAGE_MAGIC 0xDE
#define STORAGE_START_ADDR 0

extern Settings CurrentSettings;

void Storage_Init()
{
    uint8_t magic;
    EEPROM.get(STORAGE_START_ADDR, magic);

    if (magic != STORAGE_MAGIC)
    {
        Serial.println(F("Storage: No valid data found. Resetting to defaults..."));
        Storage_Reset();
    }
    else
    {
        Serial.println(F("Storage: Data loaded successfully."));
        Storage_LoadSettings(&CurrentSettings);
    }
}

void Storage_SaveSettings(const Settings *settings)
{
    uint8_t magic = STORAGE_MAGIC;
    EEPROM.put(STORAGE_START_ADDR, magic);

    EEPROM.put(STORAGE_START_ADDR + sizeof(magic), *settings);

    Serial.println(F("Storage: Settings save to EEPROM."));
}

bool Storage_LoadSettings(Settings *settings)
{
    uint8_t magic;
    EEPROM.get(STORAGE_START_ADDR, magic);

    if (magic == STORAGE_MAGIC)
    {
        EEPROM.get(STORAGE_START_ADDR + sizeof(magic), *settings);
        return true;
    }
    return false;
}

void Storage_Reset()
{
    CurrentSettings.ceilingBrightness = 150;
    CurrentSettings.lastCeilingMode = CEILING_MODE_CALM;
    CurrentSettings.staticColorPreset = COLOR_PRESET_COLD;

    CurrentSettings.hue = 0;
    CurrentSettings.saturation = 0;
    CurrentSettings.value = 255;

    CurrentSettings.monitorBrightness = 128;
    CurrentSettings.monitorEnabled = false;

    CurrentSettings.tempOffset = 0.0;
    CurrentSettings.humOffset = 0.0;
    CurrentSettings.displayContrast = 60;

    Storage_SaveSettings(&CurrentSettings);
}