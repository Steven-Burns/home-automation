#pragma once
#include <cstddef> // size_t
#include <api/Common.h>


typedef unsigned long ulong;



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

    static const unsigned LONG_PRESS_THRESHOLD_MS = 600;
    static const unsigned DOUBLE_PRESS_THRESHOLD_MS = 350;
    static const unsigned DEBOUNCE_THRESHOLD_MS = 750;

    States state = States::WaitingForHigh;
    KeyPressKind kind = KeyPressKind::None;
    ulong longPressTimerStart = 0;
    ulong dblPressTimerStart = 0;   
    ulong debounceTimerStart = 0;

public:
    KeyPressKind Update(PinStatus status);
    const char *ToString() const;
};
