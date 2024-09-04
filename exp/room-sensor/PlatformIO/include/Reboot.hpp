#pragma once

class Reboot
{
public:
    static void SetRebootTimeout(ulong millis);
    static void RebootIfTime();
};
