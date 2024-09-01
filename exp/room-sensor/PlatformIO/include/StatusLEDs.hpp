#pragma once

// One place where board makers try to innovate is in geegaws like builtin LEDs. This class
// attempts to abstract that away.

class StatusLEDs
{
public:
    static void Setup();
    static void Clear();
    static void TogglePrimary();
    static void ToggleSecondary();
};