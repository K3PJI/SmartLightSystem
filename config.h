#pragma once
#include <Arduino.h>

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

#define PIN_DHT 4

#define PIN_LCD_CS 10
#define PIN_LCD_RS 9
#define PIN_LCD_SCL 13
#define PIN_LCD_SI 11
#define PIN_LCD_BACKLIGHT -1

#define HOLD_TIME_MS 600
#define DEBOUNCE_TIME_MS 40

#define DISPLAY_UPDATE_MS 33

#define SENSORS_UPDATE_MS 1000

#define LED_EFFECT_UPDATE_MS 20

struct SystemState
{
    SystemMode mode = MODE_MAIN_MENU;

    uint8_t led_brightness = 128;
    uint8_t monitor_brightness = 128;
    bool monitor_enabled = false;
    bool monitor_white_selected = true;

    uint8_t selected_menu_item = 0;
};

extern SystemState State;
