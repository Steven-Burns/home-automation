#pragma once

// Key switch interpretation code

// A control key represents a user interface button that can be 'pressed' in the following ways
//  short press
//  long press
//  double press
// This code samples pin states and timings and determines from a pin's transitions which of the
// above interpretations apply.


#include <cstddef> // size_t
#include <api/Common.h>

typedef unsigned long ulong;

/*
QUESTION to self

does the sensor interpret the meaning of the keys, or just the actions taken with them?  e.g.
LIGHT_ON / LIGHT_OFF or "BUTTON 1 SINGLE CLICK"?

probably the latter, as interpretation may need to consider information not available to the sensor, like
time of day, sunset, day of year, etc.
*/

class KeyState
{
public:
    enum KeyPressKind
    {
        None,
        SinglePress,
        LongPress,
        DoublePress
    };

private:
    enum States
    {
        WaitingForHigh,
        WaitingForLow,
        LongPressDetected,
        SinglePressDetected,
        WaitingForDoubleHigh,
        WaitingForDoubleLow,
        WaitingForExpiredDoubleLow,
        DoublePressDetected,
        Debouncing
    };

    States state = States::WaitingForHigh;
    KeyPressKind kind = KeyPressKind::None;
    ulong longPressTimerStart = 0;
    ulong dblPressTimerStart = 0;
    ulong debounceTimerStart = 0;

public:
    static const char *KeyPressKindToString(KeyPressKind k)
    {
        return KeyState::KeyPressKind::DoublePress == k ? "DoublePress" : KeyState::KeyPressKind::SinglePress == k ? "SinglePress"
                                                                      : KeyState::KeyPressKind::LongPress == k     ? "LongPress"
                                                                                                                   : "Unknown";
    }

    KeyPressKind Update(PinStatus status);
};
