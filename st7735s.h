#ifndef __ST7735S_h__
#define __ST7735S_h__

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "st7735s_compat.h"

/* undef if low on mem */
#if !defined (BUFFER) && !defined (BUFFER1) && !defined (HVBUFFER)
  #warning no buffer defined, defining BUFFER1
  #define BUFFER1
#endif

typedef enum { R0, R90, R180, R270 } rotation_t;
typedef enum { ON, OFF } idlemode_t;

typedef struct  {
    union {
        struct  {
            uint16_t r:5;
            uint16_t g:6;
            uint16_t b:5;
        } __attribute__((packed));
        uint8_t u[2];
        uint16_t u16;
    };
} __attribute__((packed)) color565_t;


extern color565_t color;
extern color565_t bg_color;
extern bool bg_transparent;

void Delay(uint32_t);
void Backlight_Pct(uint8_t p);
void ST7735S_Init(void);
void ST7735S_flush(void);
void ST7735S_Pixel(uint16_t x, uint16_t y);
void ST7735S_bgPixel(uint16_t x, uint16_t y);
void setOrientation(rotation_t r);
void setTransparent(bool);
void ST7735S_fillScreen(void);
void ST7735S_sleepIn(void);
void ST7735S_sleepOut(void);
#endif

