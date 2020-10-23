#ifndef __st7735s_compat_h__
#define __st7735s_compat_h__

#include <inttypes.h>

/* this may differ from the default 80x160 */
#define defWIDTH   160
#define defHEIGHT  80
#define defXSTART  1
#define defYSTART  26

/* #define BUFFER */
/* #define BUFFER1 */
#define HVBUFFER

#ifdef __cplusplus
extern "C" {
#endif

void SPI_Init(void);
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

#ifdef __cplusplus
} // extern "C"
#endif
#endif
