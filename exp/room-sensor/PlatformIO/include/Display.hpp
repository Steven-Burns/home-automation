#pragma once 

// See https://github.com/olikraus/u8g2/
// https://github.com/olikraus/u8g2/wiki/u8g2reference


// Some potential code size or memory size optimizations, see https://github.com/olikraus/u8g2/wiki/u8g2optimization
// #define U8G2_WITHOUT_CLIP_WINDOW_SUPPORT
// #define U8G2_WITHOUT_FONT_ROTATION
// #define U8G2_WITHOUT_UNICODE

// print will not work with one of the above uncommented.

#include <U8g2lib.h>    


// small fonts:
//  u8g2.setFont(u8g2_font_luBIS08_tf);  
// u8g2.setFont(u8g2_font_ncenB10_tr);
// u8g2.setFont(u8g2_font_6x10_tf);
// u8g2.setFont(u8g2_font_pcsenior_8r);
// u8g2.setFont(u8g2_font_3x5im_tr);
// u8g2.setFont(u8g2_font_NokiaSmallPlain_tr);
// u8g2.setFont(u8g2_font_spleen5x8_mr);


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

    inline static const uint TOP_REGION_HEIGHT = 16;

    public:

    static void Setup();
    static void StartUpdate();
    static void EndUpdate();
    static void WriteString(uint col, uint row, const char* str);
};

