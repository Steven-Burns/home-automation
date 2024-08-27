// Wraps the device here: https://wiki.seeedstudio.com/Grove-Light_Sensor/

#pragma once


#include "Arduino.h"



class GroveLightSensor
{
    public:

    enum LightLevel
    {
        Dark,
        Dim,
        Bright
    };

    static const char* LightLevelToString(LightLevel l) 
    {
        return 
            LightLevel::Dark == l ? "Dark" :
                LightLevel::Dim == l ? "Dim" : 
                    LightLevel::Bright == l ? "Bright" :
                        "Unknown";
    }
    
    GroveLightSensor(uint8_t analogPin);
    LightLevel ReadLevel() const ;
    uint ReadRawLevel() const;

    private:

    uint8_t pin;
};

