#ifndef __st7735s_compat_h__
#define __st7735s_compat_h__

#include <inttypes.h>

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

#include "stm32f0xx_ll_gpio.h"

#define Pin_Init(name) LL_GPIO_SetPinMode(name ## _GPIO_Port, name ## _Pin, LL_GPIO_MODE_OUTPUT)
#define Pin_Toggle(name) LL_GPIO_TogglePin(name ## _GPIO_Port, name ## _Pin)
#define Pin_High(name) LL_GPIO_SetOutputPin(name ## _GPIO_Port, name ## _Pin);
#define Pin_Low(name) LL_GPIO_ResetOutputPin(name ##_GPIO_Port, name ## _Pin);

#endif
