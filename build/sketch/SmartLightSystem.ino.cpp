#line 1 "C:\\Users\\Kerl\\Documents\\StorageManager\\123\\SmartLightSystem\\SmartLightSystem.ino"
#include <Arduino.h>
#include <input.h>
#include <display.h>
#include <led_strip.h>
#include <sensors.h>
#include <config.h>

void void setup()
{
    Serial.begin(9600);

    input_init();
    led_init();
    display_init();
    sensors_init();
}

void loop()
{
    // Add your main code here, to run repeatedly
}
