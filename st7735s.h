#ifndef __ST7735S_h__
#define __ST7735S_h__

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

/* this may differ from the default 80x160 */
#define WIDTH  161
#define HEIGHT 106

/* undef if low on mem */
#ifndef BUFFER
    #define BUFFER 1
#endif

typedef enum { R0, R90, R180, R270 } rotation_t;
typedef enum { ON, OFF } idlemode_t;

typedef struct  {
    union {
        struct  {
            uint8_t r:5;
            uint8_t g:6;
            uint8_t b:5;
        } __attribute__((packed));
        uint8_t u[2];
        uint16_t u16;
    };
} __attribute__((__packed__)) color565_t;

extern color565_t color;
extern color565_t bg_color;
extern bool bg_transparent;

void commandsList(uint8_t cmd[], size_t size);
void emptyfunc(void);
void resetWindow(void);
void updateWindow(uint16_t x, uint16_t y);
void Delay(uint32_t);
void Backlight_Pct(uint8_t p);
void ST7735S_Init(void);
bool ST7735S_flush(void);
void ST7735S_Pixel(uint16_t x, uint16_t y);
void ST7735S_bgPixel(uint16_t x, uint16_t y);
void setTransparent(bool);
void setFont(uint8_t *);
void ST7735S_Text(uint16_t, uint16_t, char *);
void ST7735S_Line(uint16_t, uint16_t, uint16_t, uint16_t);
void ST7735S_Line2(uint16_t, uint16_t, uint16_t, uint16_t);
void ST7735S_Circle(uint16_t, uint16_t, uint16_t);
void fillScreen(void);

#endif
