#include <Arduino.h>
#include "StatusLEDs.hpp"

#if defined ARDUINO_SEEED_XIAO_RP2040
// There are the following
// - an (apparently uncontrolled) power LED
// - one each of { R G B } LEDs (in place of the Pico's single builtin)
//      Also wierd is that each of these is illuminate by pulling their respective pins HIGH.
//      While that makes more sense to me than the customary Arduino thingy of pulling LOW to
//      light up the built-in LED, why would one think to break compat for that?
// - one NeoPixel

void StatusLEDs::Setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
}

void StatusLEDs::Clear()
{
    // on the XAIO RP2040, you turn the builtin LEDs off by setting them high, weird.
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PIN_LED_B, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
}

void StatusLEDs::TogglePrimary()
{
    static int lastLEDLevel = LOW;
    lastLEDLevel = (lastLEDLevel == HIGH) ? LOW : HIGH;
    digitalWrite(PIN_LED_B, lastLEDLevel);
}

void StatusLEDs::ToggleSecondary()
{
    static int lastLEDLevel = LOW;
    lastLEDLevel = (lastLEDLevel == HIGH) ? LOW : HIGH;
    digitalWrite(PIN_LED_G, lastLEDLevel);
}

#else

void StatusLEDs::Setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
}

void StatusLEDs::Clear()
{
    digitalWrite(LED_BUILTIN, LOW);
}

void StatusLEDs::TogglePrimary()
{
    static int lastLEDLevel = HIGH;
    lastLEDLevel = (lastLEDLevel == HIGH) ? LOW : HIGH;
    digitalWrite(LED_BUILTIN, lastLEDLevel);
}

void StatusLEDs::ToggleSecondary()
{
    // TODO: decide whether other boards should get a secondary LED.
}

#endif
