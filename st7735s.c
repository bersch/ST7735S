#include <stdint.h>
#include <string.h>
#include "st7735s.h"

typedef enum {NOP       = 0x00,
              SWRESET   = 0x01, /* Software Reset */
              RDDID     = 0x04, /* Read Display ID */
              RDDST     = 0x09, /* Read Display Status */
              RDDPM     = 0x0a, /* Read Display Power Mode */
              RDDMADCTL = 0x0b, /* Read Display MADCTL */
              RDDCOLMOD = 0x0c, /* Read Display Pixel Format */
              RDDIM     = 0x0d, /* Read Display Image Mode */
              RDDSM     = 0x0e, /* Read Display Signal Mode */
              RDDSDR    = 0x0f, /* Read Display Self-Diagnostic Result */
              SLPIN     = 0x10, /* Sleep In */
              SLPOUT    = 0x11, /* Sleep Out */
              PTLON     = 0x12, /* Partial Display Mode On */
              NORON     = 0x13, /* Normal Display Mode On */
              INVOFF    = 0x20, /* Display Inversion Off */
              INVON     = 0x21, /* Display Inversion On */
              GAMSET    = 0x26, /* Gamma Set */
              DISPOFF   = 0x28, /* Display Off */
              DISPON    = 0x29, /* Display On */
              CASET     = 0x2a, /* Column Address Set */
              RASET     = 0x2b, /* Row Address Set */
              RAMWR     = 0x2c, /* Memory Write */
              RGBSET    = 0x2d, /* Color Setting 4k, 65k, 262k */
              RAMRD     = 0x2e, /* Memory Read */
              PTLAR     = 0x30, /* Partial Area */
              SCRLAR    = 0x33, /* Scroll Area Set */
              TEOFF     = 0x34, /* Tearing Effect Line OFF */
              TEON      = 0x35, /* Tearing Effect Line ON */
              MADCTL    = 0x36, /* Memory Data Access Control */
              VSCSAD    = 0x37, /* Vertical Scroll Start Address of RAM */
              IDMOFF    = 0x38, /* Idle Mode Off */
              IDMON     = 0x39, /* Idle Mode On */
              COLMOD    = 0x3a, /* Interface Pixel Format */
              RDID1     = 0xda, /* Read ID1 Value */
              RDID2     = 0xdb, /* Read ID2 Value */
              RDID3     = 0xdc, /* Read ID3 Value */
              FRMCTR1   = 0xb1, /* Frame Rate Control in normal mode, full colors */
              FRMCTR2   = 0xb2, /* Frame Rate Control in idle mode, 8 colors */
              FRMCTR3   = 0xb3, /* Frame Rate Control in partial mode, full colors */
              INVCTR    = 0xb4, /* Display Inversion Control */
              PWCTR1    = 0xc0, /* Power Control 1 */
              PWCTR2    = 0xc1, /* Power Control 2 */
              PWCTR3    = 0xc2, /* Power Control 3 in normal mode, full colors */
              PWCTR4    = 0xc3, /* Power Control 4 in idle mode 8colors */
              PWCTR5    = 0xc4, /* Power Control 5 in partial mode, full colors */
              VMCTR1    = 0xc5, /* VCOM Control 1 */
              VMOFCTR   = 0xc7, /* VCOM Offset Control */
              WRID2     = 0xd1, /* Write ID2 Value */
              WRID3     = 0xd2, /* Write ID3 Value */
              NVFCTR1   = 0xd9, /* NVM Control Status */
              NVFCTR2   = 0xde, /* NVM Read Command */
              NVFCTR3   = 0xdf, /* NVM Write Command */
              GMCTRP1   = 0xe0, /* Gamma '+'Polarity Correctio Characteristics Setting */
              GMCTRN1   = 0xe1, /* Gamma '-'Polarity Correctio Characteristics Setting */
              GCV       = 0xfc, /* Gate Pump Clock Frequency Variable */
} ST7735S_Command;


#if BUFFER
#define FRAMESIZE (WIDTH*HEIGHT)
color565_t frame[FRAMESIZE] = {0};
#else
#define FRAMESIZE 1
color565_t frame[FRAMESIZE] = {0};
#endif

uint16_t xmin, xmax, ymin, ymax;

/* colors */
color565_t color;
color565_t bg_color;
bool       bg_transparent = false;

/* columns: 1 = # of params, 2 = command, 3 .. = params */
static uint8_t init_cmd[] = {
    1, SWRESET, /* software reset */
    1,  SLPOUT, /* sleep out, turn off sleep mode */
    4, FRMCTR1, 0x00, 0b111111, 0b111111, /* frame frequency normal mode (highest frame rate in normal mode) */
    4, FRMCTR2, 0b1111, 0x01, 0x01, /* frame frequency idle mode */
    7, FRMCTR3, 0x05, 0x3c, 0x3c, 0x05, 0x3c, 0x3c,  /* frame freq partial mode: 1-3 dot inv, 4-6 col inv */
    2,  INVCTR, 0x03, /* display inversion control: 3-bit 0=dot, 1=col */
    4,  PWCTR1, 0x28, 0x08, 0x04, /* power control */
    2,  PWCTR2, 0xc0,
    3,  PWCTR3, 0x0d, 0x00,
    3,  PWCTR4, 0x8d, 0x2a,
    3,  PWCTR5, 0x8d, 0xee, /* partial */
    2,  VMCTR1, 0x1a, /* vcom voltage */
    2,  MADCTL, 0b01100000, /* read write scanning direction */
    2,  COLMOD, 0x05, /* 3=12bit, 5=16-bit, 6=18-bit  pixel color mode */
    1,   INVON, /* display inversion on/off */
    2, NVFCTR1, 0b01000000, /* automatic adjust gate pumping clock for saving power consumption */
    2, VMOFCTR, 0b11111, /* Set VCOM Voltage level for reduce the flicker issue */
    5,   CASET, 0x00, 0x00, 0x00,  WIDTH-1, /* column (in frame mem) address set: 2 16bit values XS, XE */
    5,   RASET, 0x00, 0x00, 0x00, HEIGHT-1, /* row (in frame mem) address set: 2 16bit values YS, YE */
    2,  GAMSET, 0x02,
    1,  IDMOFF,
    1,   NORON,  /* normal display mode on */
    1,  DISPON,  /* recover from display off, output from frame mem enabled */
};

void initCommands(void) {

	uint8_t args;

    Pin_CS_Low();
    for(uint16_t i = 0; i < sizeof(init_cmd); i+=args+1) {
        args = init_cmd[i];

        SPI_Transmit(args, &init_cmd[i+1]);
    }
    Pin_CS_High();
}

void setOrientation(rotation_t r) {
    uint8_t cmd[] = { MADCTL, 0 };
    switch ((uint8_t)r) {
        case   R0: cmd[1] = 0b01100000; break;
        case  R90: cmd[1] = 0b11000000; break;
        case R180: cmd[1] = 0b10100000; break;
        case R270: cmd[1] = 0b01000000; break;
    }
    Pin_CS_Low();
    SPI_Transmit( 2, cmd);
    Pin_CS_High();
}

void setTransparent(bool t) {
    bg_transparent = t;
}

void resetWindow(void) {
    xmin = WIDTH - 1; xmax = 0; ymin = HEIGHT - 1; ymax = 0;
}

void updateWindow(uint16_t x, uint16_t y) {

    if (x < xmin) xmin = x;
    if (x > xmax) xmax = x;
    if (y < ymin) ymin = y;
    if (y > ymax) ymax = y;
}

void ST7735S_fillScreen(void) {
#if BUFFER

    for(uint16_t y = 0; y < HEIGHT; y++)
        for(uint16_t x = 0; x < WIDTH; x++)
            ST7735S_Pixel(x,y);
    ST7735S_flush();

#else
    uint8_t cmd[] = { RAMWR };
    color565_t buffer[WIDTH];
    for(uint16_t x = 0; x < WIDTH; x++)
        buffer[x] = color;

    Pin_CS_Low();

    for (uint16_t y = 0; y < HEIGHT; y ++) {
        uint8_t c1[] = { CASET, 0 >> 8, 0, WIDTH >> 8, WIDTH };
        uint8_t c2[] = { RASET, y >> 8, y, y >> 8, y };
        SPI_Transmit(sizeof(c1), c1);
        SPI_Transmit(sizeof(c2), c2);
        SPI_TransmitCmd(1, cmd);
        SPI_TransmitData( WIDTH * sizeof(color565_t), (uint8_t *)&buffer);
    }
#endif
}


void ST7735S_Init(void) {

    cInit();

    /* backlight */
    // Pin_BLK_Pct(100);

    /* hard reset */
    Pin_RES_High();
    Pin_CS_Low();
    Pin_DC_Low();
    Delay(2); /* 10µs min */
    Pin_RES_Low();
    Delay(2); /* 10µs min */
    Pin_RES_High();
    Delay(2);
    Pin_CS_High();

    initCommands();

    /* clear screen */
    ST7735S_fillScreen();
}

bool ST7735S_flush(void)
{
    uint8_t cmd[] = { RAMWR };

    if (xmax >= xmin && ymax >= ymin) {
        Pin_CS_Low();
        // ST7735S_Idle(OFF);
        uint8_t c1[] = { CASET, xmin >> 8, xmin, xmax >> 8, xmax };
        uint8_t c2[] = { RASET, ymin >> 8, ymin, ymax >> 8, ymax };

        SPI_Transmit(sizeof(c1), c1);
        SPI_Transmit(sizeof(c2), c2);
        SPI_TransmitCmd(1, cmd);

#if BUFFER
        uint16_t len  = (xmax-xmin+1)*2;
        for (uint16_t y = ymin; y <= ymax; y++)
            SPI_TransmitData(len, (uint8_t *)&frame[WIDTH*y+xmin]);
#else
        SPI_TransmitData( 2, (uint8_t *)&frame[0]);
#endif
        // ST7735S_Idle(ON);
        Pin_CS_High();
        resetWindow();
        return true;
    }
    return false;
}

#if BUFFER
void ST7735S_Pixel(uint16_t x, uint16_t y) {
    if ( x < WIDTH && y < HEIGHT) {
        frame[WIDTH*y+x] = color;
        updateWindow(x,y);
    }
}

void ST7735S_bgPixel(uint16_t x, uint16_t y) {
    if ( x < WIDTH && y < HEIGHT) {
        frame[WIDTH*y+x] = bg_color;
        updateWindow(x,y);
    }
}
#else
void ST7735S_Pixel(uint16_t x, uint16_t y) {
    if ( x < WIDTH && y < HEIGHT) {
    frame[0] = color;
    updateWindow(x,y);
    ST7735S_flush();
    }
}

void ST7735S_bgPixel(uint16_t x, uint16_t y) {
    if ( x < WIDTH && y < HEIGHT) {
    frame[0] = bg_color;
    updateWindow(x,y);
    ST7735S_flush();
    }
}
#endif

void Delay(uint32_t d) {
    _Delay(d);
}

void Backlight_Pct(uint8_t p) {
    if (p <= 100) 
        Pin_BLK_Pct(p);
}

