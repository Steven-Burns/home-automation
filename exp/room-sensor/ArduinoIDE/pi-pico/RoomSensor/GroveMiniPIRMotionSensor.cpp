#include "GroveMiniPIRMotionSensor.hpp"



GroveMotionSensor::GroveMotionSensor(uint8_t digitalPin)
{
    pin = digitalPin;
}



bool
GroveMotionSensor::IsMotionDetected() const
{
    return digitalRead(pin);
}