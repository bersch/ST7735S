#include "spi.h"
#include "tim.h"
#include "st7735s_compat.h"

uint32_t tim_period = 32768;
uint32_t tim_pulse;

void cInit(void) {
    // HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); // CS low
}

void Pin_CS_Low(void) {
}

void Pin_CS_High(void) {
    // HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

void Pin_RES_High(void) {
    Pin_High(ST_RESET);
}

void Pin_RES_Low(void) {
    Pin_Low(ST_RESET);
}

void Pin_DC_High(void) {
    Pin_High(ST_DC);
}

void Pin_DC_Low(void) {
    Pin_Low(ST_DC);
}

void Pin_BLK_Pct(uint8_t pct) {
    tim_pulse = pct*tim_period/100;
    __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, tim_pulse);
}

void SPI_send(uint16_t len, uint8_t *data) {
	while (len--)
		HAL_SPI_Transmit(&hspi1, data++, 1, 0xF000);
}

void SPI_TransmitCmd(uint16_t len, uint8_t *data) {
    Pin_DC_Low();
    SPI_send(len, data);
}

void SPI_TransmitData(uint16_t len, uint8_t *data) {
    Pin_DC_High();
    SPI_send(len, data);
}

void SPI_Transmit(uint16_t len, uint8_t *data) {
     SPI_TransmitCmd(1, data++);
     if (--len)
        SPI_TransmitData(len, data);
}

void _Delay(uint32_t d) {
    HAL_Delay(d);
}

