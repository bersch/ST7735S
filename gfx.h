#ifndef __GFX_H__
#define __GFX_H__

#ifdef __cplusplus
extern "C" {
#endif

void setPixel(uint16_t, uint16_t);
void setbgPixel(uint16_t, uint16_t);
void fillScreen(void);
void flushBuffer(void);
void setFont(uint8_t *);
void drawText(uint16_t, uint16_t, char *);
void drawCircle(uint16_t, uint16_t, uint16_t);
void filledCircle(uint16_t, uint16_t, uint16_t);
void drawLine(uint16_t,uint16_t, uint16_t, uint16_t);
void setColorRaw(color565_t);
void setbgColorRaw(color565_t);
void setColor(uint8_t, uint8_t, uint8_t);
void setbgColor(uint8_t, uint8_t, uint8_t);
void filledCircle(uint16_t, uint16_t, uint16_t);
void drawRect(uint16_t, uint16_t, uint16_t, uint16_t);
void filledRect(uint16_t, uint16_t, uint16_t, uint16_t);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

