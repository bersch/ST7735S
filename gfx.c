#include <stdint.h>
#include "st7735s.h"
#include "fonts.h"
#include "gfx.h"

#ifndef abs
#define abs(x) ( (x<0) ? -(x) : (x) )
#endif

/* transparent background for fonts */
bool bg_transparent = false;

void setTransparent(bool t) {
    bg_transparent = t;
}

void setPixel(uint16_t x, uint16_t y) {
    ST7735S_Pixel(x, y);
}
void setbgPixel(uint16_t x, uint16_t y) {
    ST7735S_bgPixel(x, y);
}

void flushBuffer(void) {
    ST7735S_flush();
}

/******************************************************************************
  Line+Circle // Bresenham's algorithm
 ******************************************************************************/

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

void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

    uint16_t abs_y = abs(y1 - y0);
    uint16_t abs_x = abs(x1 - x0);

    if (abs_y <= abs_x) {
        if (x0 > x1)
            _LineLow(x1, y1, x0, y0);
        else
            _LineLow(x0, y0, x1, y1);
    }
    if (abs_y >= abs_x) {
        if (y0 > y1)
            _LineHigh(x1, y1, x0, y0);
        else
            _LineHigh(x0, y0, x1, y1);
    }
}

/******************************************************************************
  Circle
 *******************************************************************************/

void plot8CirclePoints(uint16_t xc, uint16_t yc, uint16_t x, uint16_t y) {

    ST7735S_Pixel(xc+x, yc+y);
    ST7735S_Pixel(xc+y, yc+x);
    ST7735S_Pixel(xc-y, yc+x);
    ST7735S_Pixel(xc-x, yc+y);

    ST7735S_Pixel(xc-x, yc-y);
    ST7735S_Pixel(xc-y, yc-x);
    ST7735S_Pixel(xc+y, yc-x);
    ST7735S_Pixel(xc+x, yc-y);
}

void drawCircle(uint16_t xc, uint16_t yc, uint16_t r) {

    int16_t x, y, err;

    x = r; y = err = 0;

    while (x >= y) {
        plot8CirclePoints(xc,yc,x,y);
        if ( err > 0)
        {  x--; err -= 2*x + 1; }
        else
        {  y++; err += 2*y + 1; }
    }
}

void filledCircle(uint16_t xc, uint16_t yc, uint16_t r) {

    int16_t x, y, err;

    x = r; y = err = 0;

    while (x >= y) {

        drawLine(xc-x,yc-y,xc+x,yc-y);
        drawLine(xc-x,yc+y,xc+x,yc+y);
        drawLine(xc-y,yc+x,xc+y,yc+x);
        drawLine(xc-y,yc-x,xc+y,yc-x);

        if ( err > 0)
        {  x--; err -= 2*x + 1; }
        else
        {  y++; err += 2*y + 1; }
    }
}

/******************************************************************************
  Arc & Pie
 *******************************************************************************/

#define PI   3.141592654f
#define PI4  PI/4.0
#define PI34 PI*3.0/4.0

float Atan2(int16_t y, int16_t x)
{
    float angle;
    uint16_t abs_y = abs(y);

    if ( x == 0 && y == 0)
        return 0;

    if (x>=0) {
        if (abs_y + x == 0)
            return 0;
        angle = PI4  - ( PI4 * (1.0 * (x - abs_y) / (1.0 * (abs_y + x))) );
    } else {
        if (abs_y - x == 0)
            return 0;
        angle = PI34 - ( PI4 * (1.0 * (x + abs_y) / (1.0 * (abs_y - x))) );
    }

    angle = 180.0 * angle / PI;
    return (y<0) ? -angle:angle;
}



bool isPie = false;
uint16_t gxc, gyc;
float    angle_from, angle_to;

void CheckAngle(uint16_t x, uint16_t y) {

    int16_t dx = x - gxc;
    int16_t dy = y - gyc;

    float angle = Atan2(dy,dx);

    if (angle < 0.0)
        angle += 360.0;

    if (angle_from <= angle_to) {
        if (angle >= angle_from && angle <= angle_to)
            (isPie) ? drawLine(gxc, gyc, x, y) : ST7735S_Pixel(x, y);
    } else {
        if (angle <= angle_to || angle >= angle_from)
            (isPie) ? drawLine(gxc, gyc, x, y) : ST7735S_Pixel(x, y);
    }
}

void plot8ArcPoints(uint16_t xc, uint16_t yc, uint16_t x, uint16_t y) {

    CheckAngle(xc+x, yc+y);
    CheckAngle(xc+y, yc+x);
    CheckAngle(xc-y, yc+x);
    CheckAngle(xc-x, yc+y);

    CheckAngle(xc-x, yc-y);
    CheckAngle(xc-y, yc-x);
    CheckAngle(xc+y, yc-x);
    CheckAngle(xc+x, yc-y);
}

void drawArc(uint16_t xc, uint16_t yc, uint16_t r, float a_from, float a_to) {

    int16_t x, y, err;

    x = r; y = err = 0;
    gxc = xc; gyc = yc;

    angle_from = a_from; angle_to = a_to;

    while (x >= y) {
        plot8ArcPoints(xc,yc,x,y);
        if ( err > 0)
        {  x--; err -= 2*x + 1; }
        else
        {  y++; err += 2*y + 1; }
    }
}

void drawPie(uint16_t xc, uint16_t yc, uint16_t r, float a_from, float a_to) {

    isPie = true;
    drawArc(xc, yc, r, a_from, a_to);
    isPie = false;
}

/******************************************************************************
  Rect
 *******************************************************************************/

void drawRect(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2) {
    drawLine(x,  y, x2,  y);
    drawLine(x, y2, x2, y2);
    drawLine(x,  y,  x, y2);
    drawLine(x2, y, x2, y2);
}

void filledRect(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2) {

    if (x > x2) { uint16_t tmp = x; x = x2; x2 = tmp; }
    if (y > y2) { uint16_t tmp = y; y = y2; y2 = tmp; }

    /* fast ergonomic grid fill */
    if ( abs(x - x2) < abs(y - y2) ) {
        uint16_t xl = x2 - ((abs(x - x2) & 1) ? 0 : 1);
        while (x < x2)  {
            drawLine( x, y, x, y2 );
            drawLine( xl, y, xl, y2 );
            x+=2;
            xl-=2;
        }
    } else {
        uint16_t yl = y2 - ((abs(y - y2) & 1) ? 0 : 1);
        while (y < y2)  {
            drawLine( x, y, x2, y );
            drawLine( x, yl, x2, yl );
            y+=2;
            yl-=2;
        }
    }
}

void fillScreen(void) {
    filledRect(0,0,WIDTH, HEIGHT);
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
        for (uint8_t x = 0; x < pfont.gi->bbox.width; x++) {
            if (x % 8 == 0)
                row = *glyph++;
            if (row & ( 1 << (7-(x % 8)))) {
                ST7735S_Pixel(xx+x, yy+h);
            } else {
                if (bg_transparent == false) {
                    ST7735S_bgPixel(xx+x, yy+h);
                }
            }
        }
    }
}

void drawText(uint16_t x, uint16_t y, const char *t) {
    while (*t) {
        drawGlyph(x,y, *t++);
        x += pfont.gi->bbox.width;
    }
}

/******************************************************************************
  Colors
 *******************************************************************************/

void setColorC(color565_t c) {
    // color.u16 = __builtin_bswap16(c.u16);
    // color.u16 = (c.g << 6 | c.r) << 8 | (c.b << 3 | c.g >> 3);
    color.u[0] = c.u[1];
    color.u[1] = c.u[0];
}

void setbgColorC(color565_t c) {
    // bg_color.u16 = __builtin_bswap16(c.u16);
    // bg_color.u16 = (c.g << 6 | c.r) << 8 | (c.b << 3 | c.g >> 3);
    // bg_color.u16 = c.u[0] << 8 | c.u[1];
    bg_color.u[0] = c.u[1];
    bg_color.u[1] = c.u[0];
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
    setColorC((color565_t){ .r = r, .g = g, .b = b });
}

void setbgColor(uint8_t r, uint8_t g, uint8_t b) {
    setbgColorC((color565_t){ .r = r, .g = g, .b = b });
}

struct s_color {
          uint8_t r:8;
          uint8_t g:8;
          uint8_t b:8;
          uint8_t  :8;
};

void setColor24(uint32_t _color) {
    struct s_color *c = (struct s_color *)&_color;
    setColorC((color565_t){ .r = c->r*32/256, .g = c->g*64/256, .b = c->b*32/256 });
}

void setbgColor24(uint32_t _color) {
    struct s_color *c = (struct s_color *)&_color;
    setbgColorC((color565_t){ .r = c->r*32/256, .g = c->g*64/256, .b = c->b*32/256 });
}

