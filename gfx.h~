#ifndef __GFX_H__
#define __GFX_H__

#ifdef __cplusplus
extern "C" {
#endif

#define C_Gray50 0x808080
#define C_Gray40 0x606060
#define C_Gray25 0x404040
#define C_Red50  0x400000

void setPixel(uint16_t, uint16_t);
/* background pixel used for font draw */
void setbgPixel(uint16_t, uint16_t);
void fillScreen(void);
/* needs to be the last action when using BUFFER or HVBUFFER */
void flushBuffer(void);
void setFont(uint8_t *);
void drawText(uint16_t, uint16_t, const char *);
void drawCircle(uint16_t, uint16_t, uint16_t);
void filledCircle(uint16_t, uint16_t, uint16_t);
void drawArc(uint16_t, uint16_t, uint16_t, float, float);
void drawPie(uint16_t, uint16_t, uint16_t, float, float);
void drawLine(uint16_t,uint16_t, uint16_t, uint16_t);
void setColorRaw(color565_t);
void setbgColorRaw(color565_t);
void setColor(uint8_t, uint8_t, uint8_t);
void setTransparent(bool);
/* background color for fonts if !transparent */
void setbgColor(uint8_t, uint8_t, uint8_t);
void drawRect(uint16_t, uint16_t, uint16_t, uint16_t);
void filledRect(uint16_t, uint16_t, uint16_t, uint16_t);
void setColor24(uint32_t);
void setbgColor24(uint32_t);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

