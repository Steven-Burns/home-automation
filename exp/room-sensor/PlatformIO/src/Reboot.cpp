#include <Arduino.h>
#include "Reboot.hpp"

// default is 4 hours

ulong rebootThreshold =
    1000 // ms per sec
    * 60 // sec per min
    * 1  // min per hr
         //   *  4  // hrs
    ;

void Reboot::SetRebootTimeout(ulong millis)
{
    rebootThreshold = millis;
}

void Reboot::RebootIfTime()
{
    if (millis() > rebootThreshold)
    {
        Serial.println("Time to die...");
        rp2040.restart();
    }
}
