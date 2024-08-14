#include "PinSampler.hpp"
#include <stdio.h>
#include <cassert>

PinSample*
PinSampler::Current() const
{
    return current;
}

PinSample*
PinSampler::Previous() const
{
    return previous;
}

void PinSampler::TakeSample(uint32_t clock, int pinValue)
{
    // swap the current and previous ptrs.  Result is that everytime this method is called, *current is the most recent
    // sample taken and *previous is the prior one
    PinSample *prev = previous;
    previous = current;
    current = prev;

    current->clock = clock;
    current->pinVal = pinValue;
}

bool PinSampler::IsSamplePinValChanged() const
{
    bool result = current->pinVal != previous->pinVal;

    // check to see that the timestamps have increased when there is a delta. If not, then why is the pin changed?
    assert(result && current->clock > previous->clock);
    return result;
}

const char *
PinSampler::ToString() const
{
    snprintf(
        (char *)toStringBuf,
        toStringBufChars,
        "c %1i %04X | p %1i %04X",
        current->pinVal, current->clock, previous->pinVal, previous->clock);
    return toStringBuf;
}
