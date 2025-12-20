#include "sensors.h"
#include "config.h"
#include <DHT.h>

DHT dht(PIN_DHT, DHT22);

extern Settings CurrentSettings;

static float currentTemp = 0.0;
static float currentHum = 0.0;
static SensorStatus lastStatus = SENSOR_ERROR;
static unsigned long lastReadTime = 0;

void Sensors_Init()
{
    dht.begin();
    Sensors_Update();
}

void Sensors_Update()
{
    unsigned long now = millis();

    if (now - lastReadTime >= SENSORS_READ_INTERVAL_MS)
    {

        float rawTemp = dht.readTemperature();
        float rawHum = dht.readHumidity();

        if (isnan(rawTemp) || isnan(rawHum))
        {
            lastStatus = SENSOR_ERROR;
            Serial.println = SENSOR_ERROR;
            Serial.println(F("Sensors: Ошибка чтения DHT22"));
        }
        else
        {
            lastStatus = SENSOR_OK;

            currentTemp = rawTemp + CurrentSettings.tempOffset;
            currentHum = rawHum + CurrentSettings.humOffset;

            currentHum = constrain(currentHum, 0.0, 100.0);
        }

        lastReadTime = now;
    }
}

float Sensors_GetTemperature()
{
    return currentTemp;
}

float Sensors_GetHumidity()
{
    return currentHum;
}

SensorStatus Sensors_GetStatus()
{
    return lastStatus;
}