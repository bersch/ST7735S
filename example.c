#include "st7735s.h"
#include "fonts.h"
#include "gfx.h"

void  example(void) {
        
        ST7735S_Init();

        Backlight_Pct(90);
        
        setColorRGB(0,0,0);
        fillScreen();
        
        setColorRGB(10,20,30);
        drawCircle(50, 50, 80);

        setColorRGB(30,0,30);
        drawLine(80,20,140,60);

        setColorRGB(31,63,31);
        setbgColorRGB(0,0,0);
        setFont(ter_u32n);
        drawText(10,HEIGHT/2-8, "Hi World!");

        ST7735S_flush();
}
