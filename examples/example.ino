#include "st7735s.h"
#include "fonts.h"
#include "gfx.h"
#include "Arduino.h"

void example(void)   {

    uint8_t f = 0;

    while (1) {

        f = 1 - f;
        if (f) {
            setColor(0,0,0);
            fillScreen();
        } else  {

            uint8_t r = 0, g = 0, b = 0;
            for (int y = 0; y < HEIGHT; y++)
                for(int x = 0; x < WIDTH; x++) {
                    setColor(r,g,b);
                    setPixel(x,y);
                    r = x % 32;
                    g = (x+y) % 64;
                    b = y % 32;
                }
        }

        for (int i = 0; i < 32; i++) {
            setColor(i,i,i);
            drawLine(0,i,WIDTH-1,i);
        }
        setColor(31,63,31);
        setbgColor(0,0,0);
        setFont(ter_u16b);
        drawText(4,33, "Hi World!");
        for (int i = 0; i < 64; i++) {
            setColor(i/2,63-i,i/2);
            drawLine(WIDTH-1-i,0,WIDTH-1-i,HEIGHT-1);
        }
        for (int i = 0; i < 64; i++) {
            setColor(i/2,i,i/2);
            drawLine(0,HEIGHT-1-i,WIDTH-1,HEIGHT-1-i);
        }

        setColor(10,20,30);
        drawCircle(80, 50, 30);

        setColor(30,10,30);
        drawCircle(40, 10, 90);

        setColor(30,0,30);
        drawLine(0,0,WIDTH-1,HEIGHT-1);

        flushBuffer();

    }
}

void setup(void) {
    ST7735S_Init();
    setOrientation(R0);

}

void loop(void) {
    example();
}
