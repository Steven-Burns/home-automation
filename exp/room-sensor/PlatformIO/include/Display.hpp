#pragma once

#include <U8g2lib.h>

class Display
{
private:
    // This is the display for the Seeed "Yellow and Blue" module.
    // See: https://github.com/Seeed-Studio/Seeed_Learning_Space/tree/master/Grove%20-%20OLED%20Display%200.96''(SSD1315)V1.0
    // https://wiki.seeedstudio.com/Grove-OLED-Display-0.96-SSD1315/
    static U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2;
    static uint charHeightPx;
    static uint charWidthPx;
    static uint displayWidthPx;
    static uint displayHeightPx;
    static bool isAsleep;

public:
    static void Setup();
    static void StartUpdate();
    static void EndUpdate();
    static void WriteString(uint col, uint row, const char *str);
    static void Sleep();
    static void Wake();
};
