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

    Serial.print("System boot OK");
}

void loop()
{
    input_update();
    led_update();
    display_update();
    sensors_update();
}