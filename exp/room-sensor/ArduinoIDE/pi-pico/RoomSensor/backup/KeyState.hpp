#pragma once
#include "PinSampler.hpp"
#include <api/Common.h>

class KeyState
{
public:
    enum States
    {
        Unpressed,
        Pressed,
        PressedPreLong,
        Debouncing
    };

    enum KeyPressKind
    {
        None,
        ShortPress,
        LongPress
    };

private:
    States state = States::Unpressed;
    KeyPressKind kind = KeyPressKind::None;

    PinStatus pressedPinStatus = PinStatus::HIGH;
    unsigned duration = 0;
    static const unsigned ShortPressThreshold = 200;
    static const unsigned LongPressThreshold = 200;
    static const unsigned PostPressThreshold = 100;
    static const size_t toStringBufChars = 20;
    char toStringBuf[toStringBufChars];

public:
    void SetPressedPinStatus(PinStatus status);
    KeyPressKind Update(const PinSampler &sampler);
    const char *ToString() const;
};
