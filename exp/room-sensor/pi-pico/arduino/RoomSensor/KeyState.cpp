#include "KeyState.hpp"
#include <stdio.h>
#include <cassert>

void
KeyState::SetPressedPinStatus(PinStatus status)
{
  pressedPinStatus = status;
}


constexpr const char* 
StateToString(KeyState::States state) {
    switch (state) {
        case KeyState::States::Debouncing:      return "Debouncing";
        case KeyState::States::Pressed:         return "Pressed";
        case KeyState::States::PressedPreLong:  return "PressedPreLong";
        case KeyState::States::Unpressed:       return "Unpressed";
        default: return "Unknown";
    }
}


const char *
KeyState::ToString() const
{
    snprintf(
        (char *)toStringBuf,
        toStringBufChars,
        "s %s k %s",
        StateToString(state), "(kind)");
    return toStringBuf;
}


KeyState::KeyPressKind 
KeyState::Update(const PinSampler &sampler)
{
    KeyPressKind result = KeyPressKind::None;

    switch (state)
    {
    case States::Unpressed:
    {
        // todo not sure we need to check value changed.
        if (sampler.IsSamplePinValChanged() && sampler.Current()->pinVal == pressedPinStatus)
        {
            state = Pressed;
            duration = 0;
        }
        break;
    }
    case States::Pressed:
    {
        // todo not sure we need to check value changed.
        if (!sampler.IsSamplePinValChanged() && sampler.Current()->pinVal == pressedPinStatus)
        {
            // still pressed.

            duration += (sampler.Current()->clock - sampler.Previous()->clock);
            if (duration > ShortPressThreshold)
            {
                duration = 0;
                state = PressedPreLong;
            }
            // otherwise, we remain Pressed waiting for the threshold
        }
        else
        {
            // the pin has dropped, and we've not met the short press threshold. So
            // revert back to Unpressed ==> the pin transition is ignored.
            state = States::Unpressed;
        }
        break;
    }
    case States::PressedPreLong:
    {
        if (sampler.Current()->pinVal != pressedPinStatus)
        {
            // the pin has dropped, so proceed to debounce a short press
            kind = KeyPressKind::ShortPress;
            duration = 0;
            state = States::Debouncing;
        }
        else
        {
            // still pressed.

            duration += (sampler.Current()->clock - sampler.Previous()->clock);
            if (duration > LongPressThreshold)
            {
                duration = 0;
                kind = KeyPressKind::LongPress;
                duration = 0;
                state = States::Debouncing;
            }
        }
        break;
    }
    case States::Debouncing:
    {
        // we don't care about the pin state here, we're just waiting for time to pass and eating
        // any pin transition.
        duration += (sampler.Current()->clock - sampler.Previous()->clock);
        if (duration > PostPressThreshold)
        {
            // here we want to "enqueue" a keypress event somehow.
            result = kind;
            // Serial.print("BOOM! "); Serial.println(kind);

            // and then the next state is Unpressed
            state = States::Unpressed;
        }
        break;
    }
    default:
    {
        assert(false /* unhandled KeyState::States case */);
    }
    }
    return result;
}
