#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

enum SensorStatus
{
    SENSOR_OK,
    SENSOR_ERROR
};

void Sensors_Init();
void Sensors_Update();

float Sensors_GetTemperature();
float Sensors_GetHumidity();
SensorStatus Sensors_GetStatus();

#endif