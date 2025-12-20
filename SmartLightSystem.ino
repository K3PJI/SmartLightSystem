#include <Arduino.h>

#include "storage.h"
#include "input.h"
#include "display.h"
#include "led_strip.h"
#include "sensors.h"
#include "config.h"

SystemState State;
Settings CurrentSettings;

unsigned long lastDisplayUpdate = 0;
unsigned long lastActivityTime = 0;

void User_Activity_Detected() {
    lastActivityTime = millis();
    if (Display_IsSleeping()) {
        Display_Wake();
    }
}

void setup()
{
    Serial.begin(9600);

    Storage_LoadSettings(&CurrentSettings);

    input_init();
    led_init();
    Display_Init();
    sensors_init();

    Save_And_Apply_Settings();

    lastActivityTime = millis();

    Serial.print("System boot OK");
}

void loop()
{
    unsigned long now = millis();

    input_update();
    Menu_Manager_Update();

    if (!Display_IsSleeping() && (now - lastActivityTime >= UI_SLEEP_TIMEOUT_MS)) {
        Display_Sleep();
    }

    led_update();
    display_update();
    sensors_update();

    if (!Display_IsSleeping() && (now - lastDisplayUpdate >= DISPLAY_UPDATE_MS)) {
        Display_UpdateScreen();
        lastDisplayUpdate = now;
    }
}