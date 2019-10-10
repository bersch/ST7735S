#include <stdint.h>
#include "st7735s.h"
#include "fonts.h"
#include "gfx.h"

void setPixel(uint16_t x, uint16_t y) {
     ST7735S_Pixel(x, y);
}
void setbgPixel(uint16_t x, uint16_t y) {
     ST7735S_bgPixel(x, y);
}

void fillScreen(void) {
    ST7735S_fillScreen();
}

void flushBuffer(void) {
    ST7735S_flush();
}

/******************************************************************************
     Line+Circle // Bresenham's algorithm
 ******************************************************************************
 */

void _LineLow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

    int16_t dx = x1 - x0;
    int16_t dy = y1 - y0;
    int16_t yi = 1;

    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int16_t D = 2*dy - dx;
    uint16_t y = y0;

    for(uint16_t x = x0; x <= x1; x++) {
        ST7735S_Pixel(x,y);
        if (D > 0) {
            y += yi;
            D -= 2*dx;
        }
        D += 2*dy;
    }
}

void _LineHigh(uint16_t x0,uint16_t y0, uint16_t x1, uint16_t y1) {

    int16_t dx = x1 - x0;
    int16_t dy = y1 - y0;
    int16_t xi = 1;

    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int16_t D = 2*dx - dy;
    uint16_t x = x0;

    for (uint16_t y = y0; y < y1; y++) {
        ST7735S_Pixel(x,y);
        if (D > 0) {
            x += xi;
            D -= 2*dy;
        }
        D += 2*dx;
    }
}

#ifndef abs
#define abs(x) ( (x<0) ? -(x) : (x) )
#endif

void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

  if (abs(y1 - y0) < abs(x1 - x0)) {
    if (x0 > x1) {
      _LineLow(x1, y1, x0, y0);
    } else {
      _LineLow(x0, y0, x1, y1);
    }
  } else if (y0 > y1)
      _LineHigh(x1, y1, x0, y0);
    else
      _LineHigh(x0, y0, x1, y1);
}

void plot8CirclePoints(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

    ST7735S_Pixel(x1+x0, y1+y0);
    ST7735S_Pixel(x1+y0, y1+x0);
    ST7735S_Pixel(x1-y0, y1+x0);
    ST7735S_Pixel(x1-x0, y1+y0);

    ST7735S_Pixel(x1-x0, y1-y0);
    ST7735S_Pixel(x1-y0, y1-x0);
    ST7735S_Pixel(x1+y0, y1-x0);
    ST7735S_Pixel(x1+x0, y1-y0);
}

void drawCircle(uint16_t xc, uint16_t yc, uint16_t r) {

    int16_t x, y, err;

    x = r; y = err = 0;

    while (x >= y) {
        plot8CirclePoints(x,y,xc,yc);
        if ( err > 0)
            {  x--; err -= 2*x + 1; }
        else
            {  y++; err += 2*y + 1; }
    }
}

/******************************************************************************
  additional routines
 *******************************************************************************/

void filledCircle(uint16_t x, uint16_t y, uint16_t r) {
    for(int i = 0; i <= r; i++)
        drawCircle(x, y, i);
}

void drawRect(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2) {
	drawLine(x, y, x2, y);
	drawLine(x, y2, x2, y2);
	drawLine(x, y, x, y2);
	drawLine(x2, y, x2, y2);
}

void filledRect(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2) {

    if (x > x2) { uint16_t tmp = x; x = x2; x2 = tmp; }
    if (y > y2) { uint16_t tmp = y; y = y2; y2 = tmp; }

    while (x < x2 && y < y2)
        drawRect(x, y, x2--, y2--);

}

/******************************************************************************
     Fonts
 *******************************************************************************/

typedef struct {
    uint16_t first;
    uint16_t last;
} __attribute__((packed)) ch_range_t;

typedef struct {
    uint8_t width;
    uint8_t height;
    int8_t  x;
    int8_t  y;
} __attribute__((packed)) bbox_t;

typedef struct {
    uint8_t     pixel_size;
    bbox_t      bbox;
    ch_range_t  range[];
} __attribute__((packed)) glyph_info_t;

typedef struct {
        glyph_info_t *gi;
        uint8_t  *glyphs;
} __attribute__((packed)) font_t;

font_t pfont;

void setFont(uint8_t *f) {
    uint8_t i;

    pfont.gi = (glyph_info_t *)f;
    for(i = 0; (uint8_t)pfont.gi->range[i].first != 0 || i == 0; i++);
    pfont.glyphs = (uint8_t *)f + sizeof(glyph_info_t) + 
                i*sizeof(ch_range_t) + sizeof(uint8_t);
}

uint8_t *_lookupGlyph(uint16_t glyph) {

    uint16_t glnr = 0;

    uint8_t bwidth = (pfont.gi->bbox.width)/8;
    if (pfont.gi->bbox.width % 8)
        bwidth++;

    for(uint16_t i = 0; (uint8_t)pfont.gi->range[i].first != 0 || i == 0; i++) {
        if (glyph >= pfont.gi->range[i].first && glyph <= pfont.gi->range[i].last) {
            glnr += glyph - pfont.gi->range[i].first;
            return &pfont.glyphs[bwidth * glnr * pfont.gi->pixel_size];
        }
        glnr += pfont.gi->range[i].last - pfont.gi->range[i].first + 1;
    }
    return NULL;
}

void drawGlyph(uint16_t xx, uint16_t yy, uint16_t c) {
    uint8_t *glyph = _lookupGlyph(c);

    if (glyph == NULL)
        return;

    for(uint8_t h = 0; h < pfont.gi->pixel_size; h++) {
        uint8_t row;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
        for (uint8_t x = 0; x < pfont.gi->bbox.width; x++) {
            if (x % 8 == 0)
                row = *glyph++;
            if (row & ( 1 << (7-(x%8)))) {
                ST7735S_Pixel(xx+x, yy+h);
            } else {
                if (bg_transparent == false) {
                    ST7735S_bgPixel(xx+x, yy+h);
                }
            }
#pragma GCC diagnostic pop
        }
    }
}

void drawText(uint16_t x, uint16_t y, char *t) {
    while (*t) {
        drawGlyph(x,y, *t++);
        x += pfont.gi->bbox.width;
    }
}

/******************************************************************************
     Colors
 *******************************************************************************/

void setColorRaw(color565_t c) {
    // color.u16 = __builtin_bswap16(c.u16);
    // color.u16 = (c.g << 6 | c.r) << 8 | (c.b << 3 | c.g >> 3);
    color.u[0] = c.u[1];
    color.u[1] = c.u[0];
}

void setbgColorRaw(color565_t c) {
    // bg_color.u16 = __builtin_bswap16(c.u16);
    // bg_color.u16 = (c.g << 6 | c.r) << 8 | (c.b << 3 | c.g >> 3);
    // bg_color.u16 = c.u[0] << 8 | c.u[1];
    bg_color.u[0] = c.u[1];
    bg_color.u[1] = c.u[0];
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
    setColorRaw((color565_t){ .r = r, .g = g, .b = b });
}
void setbgColor(uint8_t r, uint8_t g, uint8_t b) {
    setbgColorRaw((color565_t){ .r = r, .g = g, .b = b });
}

