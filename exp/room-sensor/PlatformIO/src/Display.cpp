#include "Display.hpp"

// U8G2_SSD1306_128X64_NONAME_F_HW_I2C is the "I2C in hardware" version. It doesn't
// work on the XAIO RP2040.
U8G2_SSD1306_128X64_NONAME_F_SW_I2C Display::u8g2(U8G2_R0, 7, 6, U8X8_PIN_NONE);
uint Display::charHeightPx = 0;
uint Display::charWidthPx = 0;
uint Display::displayWidthPx = 0;
uint Display::displayHeightPx = 0;

// the number of pixel rows that are yellow on the display. 
inline static const uint TOP_REGION_HEIGHT = 16;



void Display::Setup()
{
    u8g2.begin();
    u8g2.setFont(u8g2_font_tinypixie2_tr); // smallest so far
    u8g2.setFontMode(0);
    u8g2.setDrawColor(1);
    u8g2.setContrast(255);
    // u8g2.setAutoPageClear(1);
    displayWidthPx = u8g2.getDisplayWidth();
    displayHeightPx = u8g2.getDisplayHeight();
    charHeightPx = u8g2.getMaxCharHeight();
    charWidthPx = u8g2.getMaxCharWidth();
}

void Display::WriteString(uint col, uint row, const char *str)
{
    u8g2.drawStr(charWidthPx * col + 2, charHeightPx * (row + 1) + 3 * (row > 1), str);
}

void Display::StartUpdate()
{
    u8g2.clearBuffer();
}

void Display::EndUpdate()
{
    u8g2.drawFrame(0, 0, displayWidthPx - 1, TOP_REGION_HEIGHT);
    u8g2.drawFrame(
        0, TOP_REGION_HEIGHT + 1,
        displayWidthPx - 1, displayHeightPx - TOP_REGION_HEIGHT - 1);

    u8g2.sendBuffer();
}
