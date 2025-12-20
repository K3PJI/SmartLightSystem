#include "led_strip.h"
#include "config.h"

#include <FastLED.h>

CRGB leds[LED_COUNT];

extern Settings CurrentSettings;

static unsigned long lastEffectUpdate = 0;
static uint8_t effectStep = 0;

void Led_Init()
{
    FastLed.addLeds<WS2812B, PIN_WS2812B, GRB>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(CurrentSettings.ceilingBrightness);

    pinMode(PIN_MONITOR_WHITE, OUTPUT);
    pinMode(PIN_MONITOR_BLUE, OUTPUT);

    Led_SetMonitorBrightness(CurrentSettings.monitorBrightness);
}

static void Effect_Calm()
{
    CRGB color;
    if (CurrentSettings.staticColorPreset == COLOR_PRESET_COLD)
    {
        color = CRGB::AliceBlue;
    }
    else if (CurrentSettings.staticColorPreset == COLOR_PRESET_WARM)
    {
        color = CRGB::WarmWhite;
    }
    else
    {
        color = CHSV(CurrentSettings.hue >> 8, CurrentSettings.saturation, CurrentSettings.value);
    }
    fill_solid(leds, LED_COUNT, color);
}

static void Effect_Rainbow()
{
    fill_rainbow(leds, LED_COUNT, effectStep, 7);
}

static void Effect_Impulse()
{
    fadeToBlackBy(leds, LED_COUNT, 20);
    int pos = beatsin16(13, 0, LED_COUNT - 1);
    leds[pos] += CHSV(effectStep, 255, 192);
}

void Led_Update()
{
    unsigned long now = millis();

    if (now - lastEffectUpdate >= LED_EFFECT_UPDATE_MS)
    {
        lastEffectUpdate = now;
        effectStep++;

        switch (CurrentSettings.lastCeilingMode)
        {
        case CEILING_MODE_CALM:
            Effect_Calm();
            break;

        case CEILING_MODE_RAINBOW:
            Effect_Rainbow();
            break;

        case CEILING_MODE_IMPULSE:
            Effect_Impulse();
            break;

        default:
            break;
        }

        FastLED.show();
    }
}

void Led_SetCeilingBrightness(uint8_t brightness)
{
    FastLED.setBrightness(brightness);
}

void Led_SetMonitorBrightness(uint8_t brightness)
{
    if (!CurrentSettings.monitorEnabled)
    {
        analogWrite(PIN_MONITOR_WHITE, 0);
        analogWrite(PIN_MONITOR_BLUE, 0);
        return;
    }

    if (CurrentSettings.monitorWhiteSelected)
    {
        analogWrite(PIN_MONITOR_WHITE, brightness);
        analogWrite(PIN_MONITOR_BLUE, 0);
    }
    else
    {
        analogWrite(PIN_MONITOR_BLUE, brightness);
        analogWrite(PIN_MONITOR_WHITE, 0);
    }
}

void Led_SetCeilingHSV(uint16_t h, uint8_t s, uint8_t v)
{
    CurrentSettings.hue = h;
    CurrentSettings.saturation = s;
    CurrentSettings.value = v;
}

void Led_SetCeilingColorPreset(int preset)
{
    CurrentSettings.staticColorPreset = (StaticColorPreset)preset;
}

void Led_ToggleMonitorColor()
{
    CurrentSettings.monitorWhiteSelected = !CurrentSettings.monitorWhiteSelected;
    Led_SetMonitorBrightness(CurrentSettings.monitorBrightness);
}