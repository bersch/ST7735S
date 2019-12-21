#ifndef __st7735s_compat_h__
#define __st7735s_compat_h__

#include <inttypes.h>

/* this may differ from the default 80x160 */
#define WIDTH   160
#define HEIGHT  80
#define XSTART  1
#define YSTART  26

/* #define BUFFER */
/* #define BUFFER1 */
#define HVBUFFER

void cInit(void);
void Pin_CS_Low(void);
void Pin_CS_High(void);
void Pin_RES_High(void);
void Pin_RES_Low(void);
void Pin_DC_High(void);
void Pin_DC_Low(void);
void Pin_BLK_Pct(uint8_t);

void SPI_TransmitCmd(uint16_t len, uint8_t *data);
void SPI_TransmitData(uint16_t len, uint8_t *data);
void SPI_Transmit(uint16_t len, uint8_t *data);
void _Delay(uint32_t d);

#endif
