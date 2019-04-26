#ifndef __GFX_H__
#define __GFX_H__

void setPixel(uint16_t, uint16_t);
void setbgPixel(uint16_t, uint16_t);
void fillScreen(void);
void flushBuffer(void);
void setFont(uint8_t *);
void drawText(uint16_t, uint16_t, char *);
void drawCircle(uint16_t, uint16_t, uint16_t);
void drawLine(uint16_t,uint16_t, uint16_t, uint16_t);
void setColor(color565_t);
void setbgColor(color565_t);
void setColorRGB(uint8_t, uint8_t, uint8_t);
void setbgColorRGB(uint8_t, uint8_t, uint8_t);
#endif
