#pragma once
#include <Arduino.h>
#include <storage.h>

#define ENC_A 2
#define ENC_B 7
#define ENC_SW A0
#define BTN A1

#define PIN_WS2812B 6
#define LED_COUNT 180

#define PIN_MONITOR_WHITE 3
#define PIN_MONITOR_BLUE 5

#define MONITOR_PWM_MIN 0
#define MONITOR_PWM_MAX 255
#define MAX_BRIGHTNESS 255

#define PIN_DHT 4

#define PIN_LCD_CS 10
#define PIN_LCD_RS 9
#define PIN_LCD_SCL 13
#define PIN_LCD_SI 11
#define PIN_LCD_BACKLIGHT -1

#define HOLD_TIME_MS 700
#define DEBOUNCE_TIME_MS 40

#define UI_SLEEP_TIMEOUT_MS (5 * 60 * 1000UL)

#define DISPLAY_UPDATE_MS 25
#define SENSORS_READ_INTERVAL_MS 5000
#define LED_EFFECT_UPDATE_MS 20

enum SystemMode
{
    MODE_START_SCREEN,
    MODE_MAIN_MENU,

    MODE_LIGHT_SETTINGS,
    MODE_CEILING_MODE_SELECTION,
    MODE_STATIC_COLOR_SELECTION,
    MODE_BRIGHTNESS_ADJUST,

    MODE_COLOR_PICKER,
    MODE_COLOR_PICKER_HUE,
    MODE_COLOR_PICKER_SAT,
    MODE_COLOR_PICKER_VAL,

    MODE_MONITOR_SETTINGS,

    MODE_SETTINGS_MENU,
    MODE_CALIBRATION_MENU,
    MODE_CALIBRATION_TEMP,
    MODE_CALIBRATION_HUM,
    MODE_CALIBRATION_ADJUST,
    MODE_ABOUT
};

enum CeilingMode
{
    CEILING_MODE_CALM,
    CEILING_MODE_RAINBOW,
    CEILING_MODE_IMPULSE,
    CEILING_MODE_COUNT
};

enum StaticColorPreset
{
    COLOR_PRESET_COLD,
    COLOR_PRESET_WARM,
    COLOR_PRESET_CUSTOM,
    COLOR_PRESET_COUNT
};

struct Settings
{
    uint8_t ceilingBrightness;
    CeilingMode lastCeilingMode;
    StaticColorPreset staticColorPreset;

    uint16_t hue;
    uint8_t saturation;
    uint8_t value;

    uint8_t monitorBrightness;
    bool monitorEnabled;
    bool monitorWhiteSelected;

    float tempOffset;
    float humOffset;
    uint8_t displayContrast;
};

struct SystemState
{
    SystemMode mode = MODE_START_SCREEN;

    uint8_t selected_menu_item = 0;

    bool needsUpdate = true;
};