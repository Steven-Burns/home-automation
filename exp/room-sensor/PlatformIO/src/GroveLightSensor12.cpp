#include "GroveLightSensor12.hpp"

GroveLightSensor::GroveLightSensor(uint8_t analogPin)
{
    pin = analogPin;
}

GroveLightSensor::LightLevel
GroveLightSensor::ReadLevel() const
{
    uint l = ReadRawLevel();
    GroveLightSensor::LightLevel result = Dark;

    if (l > 150)
    {
        result = Bright;
    }
    else if (l > 50)
    {
        result = Dim;
    }
    return result;
}

uint GroveLightSensor::ReadRawLevel() const
{
    return analogRead(pin);
}