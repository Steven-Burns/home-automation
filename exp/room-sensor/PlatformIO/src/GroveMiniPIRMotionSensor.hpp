// Wraps the device here: https://wiki.seeedstudio.com/Grove-PIR_Motion_Sensor/

#pragma once


#include "Arduino.h"



class GroveMotionSensor
{
    public:

    GroveMotionSensor(uint8_t digitalPin);
    bool IsMotionDetected() const;

    private:

    uint8_t pin;
};

