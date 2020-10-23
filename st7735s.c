#include <stdint.h>
#include "st7735s.h"
#include "st7735s_compat.h"

typedef enum {
    NOP       = 0x00,
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
              GMCTRP1   = 0xe0, /* Gamma '+'Polarity Correction Characteristics Setting */
              GMCTRN1   = 0xe1, /* Gamma '-'Polarity Correction Characteristics Setting */
              GCV       = 0xfc, /* Gate Pump Clock Frequency Variable */
} ST7735S_Command;


#if defined (BUFFER)
#define FRAMESIZE (defWIDTH*defHEIGHT)
color565_t frame[FRAMESIZE] = {0};
#elif defined (BUFFER1)
#define FRAMESIZE 1
color565_t frame[FRAMESIZE] = {0};
#elif defined (HVBUFFER)
color565_t hvframe[defWIDTH] = {0};
color565_t hvcolor1;
typedef enum { HF, VF, ONE, NONE } hvtype_t;
hvtype_t hvtype = NONE;
#else
#error buffer mode not defined
#endif

uint16_t WIDTH = defWIDTH, HEIGHT = defHEIGHT;
uint16_t XSTART = defXSTART, YSTART = defYSTART;

uint16_t xmin, xmax, ymin, ymax;
uint8_t madctl;

/* colors */
color565_t color;
color565_t bg_color;

/* columns: 1 = # of params, 2 = command, 3 .. = params */
static uint8_t init_cmd[] = {
    1, SWRESET, /* software reset */
    1,  SLPOUT, /* sleep out, turn off sleep mode */
	1, DISPOFF,  /*  output from frame mem disabled */
    4, FRMCTR1, 0x00, 0b111111, 0b111111, /* frame frequency normal mode (highest frame rate in normal mode) */
    4, FRMCTR2, 0b1111, 0x01, 0x01, /* frame frequency idle mode */
    7, FRMCTR3, 0x05, 0x3c, 0x3c, 0x05, 0x3c, 0x3c,  /* frame freq partial mode: 1-3 dot inv, 4-6 col inv */
    2,  INVCTR, 0x03, /* display inversion control: 3-bit 0=dot, 1=col */

    4,  PWCTR1, 0b11111100, 0x08, 0b10, /* power control */
    2,  PWCTR2, 0xc0,
    3,  PWCTR3, 0x0d, 0x00,
    3,  PWCTR4, 0x8d, 0x2a,
    3,  PWCTR5, 0x8d, 0xee, /* partial */

	/* display brightness and gamma */
    2,     GCV, 0b11011000, /* auto gate pump freq, max power save */
    2, NVFCTR1, 0b01000000, /* automatic adjust gate pumping clock for saving power consumption */
	2,  VMCTR1, 0b001111,  /* VCOM voltage setting */
    2, VMOFCTR, 0b10000, /* ligthness of black color 0-0x1f */
    2,  GAMSET, 0x08, /* gamma 1, 2, 4, 8 */

    2,  MADCTL, 0b01100000, /* row oder, col order, row colum xchange, vert refr order, rgb/bgr, hor refr order, 0, 0 */
    2,  COLMOD, 0x05, /* 3=12bit, 5=16-bit, 6=18-bit  pixel color mode */
    17, GMCTRP1,0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2c,
                0x29, 0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10,
    17, GMCTRN1,0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2c,
                0x2e, 0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10,
    5,   CASET, 0, 0, 0, defHEIGHT-1,
    5,   RASET, 0, 0, 0, defWIDTH-1,
    1,   INVON, /* display inversion on/off */
    1,  IDMOFF, /* idle mode off */
    1,   NORON,  /* normal display mode on */
    1,  DISPON,  /* recover from display off, output from frame mem enabled */
};

void initCommands(void) {
    uint8_t args;

    for(uint16_t i = 0; i < sizeof(init_cmd); i+=args+1) {
        args = init_cmd[i];

        SPI_Transmit(args, &init_cmd[i+1]);
    }
}

uint8_t backlight_pct;

void ST7735S_sleepIn(void) {

    uint8_t pct = backlight_pct;
    Pin_BLK_Pct(0);
    backlight_pct = pct;
    uint8_t cmd[] = { DISPOFF, SLPIN };
    SPI_TransmitCmd(2, cmd);
}

void ST7735S_sleepOut(void) {
    Pin_BLK_Pct(backlight_pct);
    uint8_t cmd[] = { SLPOUT, DISPON };
    SPI_TransmitCmd(2, cmd);
}

void setOrientation(rotation_t r) {

    switch ((uint8_t)r) {
        case   R0: { madctl = 0b01100000;
            WIDTH = defWIDTH; HEIGHT = defHEIGHT;
            XSTART = defXSTART; YSTART = defYSTART;
            break;
        }
        case  R90: { madctl = 0b11000000;
            WIDTH = defHEIGHT; HEIGHT = defWIDTH;
            XSTART = defYSTART; YSTART = defXSTART;
            break;
        }
        case R180: { madctl = 0b10100000;
            WIDTH = defWIDTH; HEIGHT = defHEIGHT;
            XSTART = defXSTART; YSTART = defYSTART;
            break;
        }
        case R270: { madctl = 0b01000000;
            WIDTH = defHEIGHT; HEIGHT = defWIDTH;
            XSTART = defYSTART; YSTART = defXSTART;
            break;
        }
    }

    uint8_t cmd[] = { MADCTL, madctl };
    SPI_Transmit( 2, cmd);
}

void resetWindow(void) {
    xmin = WIDTH - 1; xmax = 0; ymin = HEIGHT - 1; ymax = 0;
}

void updateWindow(uint16_t x, uint16_t y) {

    if (x < WIDTH && y < HEIGHT) {
        if (x < xmin) xmin = x;
        if (x > xmax) xmax = x;
        if (y < ymin) ymin = y;
        if (y > ymax) ymax = y;
    }
}

void ST7735S_Init(void) {

    SPI_Init();

    /* backlight */
    Pin_BLK_Pct(100);

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

}

void ST7735S_flush(void)
{
        uint16_t xm = xmin + XSTART, ym = ymin + YSTART;
        uint16_t xx = xmax + XSTART, yx = ymax + YSTART;

        uint8_t cas[] = { CASET, xm >> 8, xm, xx >> 8, xx };
        uint8_t ras[] = { RASET, ym >> 8, ym, yx >> 8, yx };
        uint8_t ram[] = { RAMWR };

        SPI_Transmit(sizeof(cas), cas);
        SPI_Transmit(sizeof(ras), ras);
        SPI_TransmitCmd(1, ram);

#if defined(BUFFER)
#if 1
    uint16_t len  = (xmax-xmin+1)*2;
    for (uint16_t y = ymin; y <= ymax; y++)
        SPI_TransmitData(len, (uint8_t *)&frame[WIDTH*y+xmin]);
#else
    uint16_t len = (xmax-xmin+1)*2*(ymax-ymin+1);
    SPI_TransmitData(len, (uint8_t *)&frame[WIDTH*ymin+xmin]);
#endif
#elif defined(HVBUFFER)
    if (hvtype == VF) { // horiz line
        uint16_t len  = (xmax-xmin+1)*2;
        SPI_TransmitData(len, (uint8_t *)&hvframe[xmin]);
    } else
        if (hvtype == HF) { // vert line
        uint16_t len  = (ymax-ymin+1)*2;
        SPI_TransmitData(len, (uint8_t *)&hvframe[ymin]);
    } else
    	if (hvtype == ONE) { // single pixel
    		SPI_TransmitData(2, (uint8_t *)&hvcolor1);
    	}
    hvtype = NONE;
#elif defined(BUFFER1)
    SPI_TransmitData( 2, (uint8_t *)&frame[0]);
#else
#error buffer not defined.
#endif
    resetWindow();
}

#if defined(BUFFER)
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
#elif defined(HVBUFFER)
void set_hvpixel(uint16_t x, uint16_t y) {
	// first pixel
	if (hvtype == NONE) {
first_pixel:
		hvcolor1 = color;
		hvtype = ONE;
		updateWindow(x,y);
		return;
	}
	// second pixel
	if (hvtype == ONE) {
		if (y == ymax && y == ymin && (x == xmin - 1 || x == xmax + 1))  {
			hvtype = VF;
			hvframe[xmin] = hvcolor1;
			hvframe[x] = color;
			updateWindow(x,y);
			return;
		}
		if (x == xmax && x == xmin && (y == ymin - 1 || y == ymax + 1)) {
			hvtype = HF;
			hvframe[ymin] = hvcolor1;
			hvframe[y] = color;
			updateWindow(x,y);
			return;
		}
		ST7735S_flush();
        goto first_pixel;
	}
    // third+ pixel
	if (hvtype == VF) { // horiz line
		if ( y == ymax && y == ymin && ( x >= xmin - 1 && x <= xmax + 1 )) {
			hvframe[x] = color;
			updateWindow(x,y);
			return;
		}
		ST7735S_flush();
        goto first_pixel;
	}
	if (hvtype == HF) { // vert line
		if ( x == xmax && x == xmin && ( y >= ymin - 1 && y <= ymax + 1 )) {
			hvframe[y] = color;
			updateWindow(x,y);
			return;
		}
		ST7735S_flush();
        goto first_pixel;
	}
}

void ST7735S_Pixel(uint16_t x, uint16_t y) {
    if ( x < WIDTH && y < HEIGHT) {
        set_hvpixel(x, y);
    }
}

void ST7735S_bgPixel(uint16_t x, uint16_t y) {
    if ( x < WIDTH && y < HEIGHT) {
        color565_t c = color;
        color = bg_color;
        set_hvpixel(x, y);
        color = c;
    }
}

#elif defined(BUFFER1)
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

bool ST7735S_defineScrollArea(uint16_t x, uint16_t x2) {

    /* tfa: top fixed area: nr of line from top of the frame mem and display) */
    uint16_t tfa = WIDTH-x2+XSTART;
    /* vsa: height of the vertical scrolling area in nr of line of the frame mem
       (not the display) from the vertical scrolling address. the first line appears
       immediately after the bottom most line of the top fixed area. */
    uint16_t vsa = x2-x+XSTART;
    /* bfa: bottom fixed are in nr of lines from bottom of the frame memory and display */
    uint16_t bfa = x+XSTART;

    if (tfa+vsa+bfa < 162)
        return false;

    /* reset mv */
    uint8_t CMD1[] = { MADCTL, madctl & ~(1<<5) };
    SPI_Transmit(sizeof(CMD1), CMD1);

    uint8_t CMD[] = { SCRLAR, tfa >> 8, tfa,
                              vsa >> 8, vsa,
                              bfa >> 8, bfa };

    SPI_Transmit(sizeof(CMD), CMD);

    return true;
}

void ST7735S_tearingOn(bool blanking_only) {

    uint8_t CMD[] = {TEON, (blanking_only)?0: 1 };
    SPI_Transmit(2, CMD);
}

void ST7735S_tearingOff(void) {

    uint8_t CMD[] = { TEOFF };
    SPI_Transmit(1, CMD);
}

void ST7735S_scroll(uint8_t line) {

    uint8_t CMD[] = {VSCSAD, 0, line};
    SPI_Transmit(3, CMD);
}

void ST7735S_normalMode(void) {
    uint8_t CMD[] = { NORON };
    SPI_Transmit(1, CMD);
    /* reset mv */
    uint8_t CMD1[] = { MADCTL, madctl };
    SPI_Transmit(sizeof(CMD1), CMD1);

}

void ST7735S_partialArea(uint16_t from, uint16_t to) {
    /* set ml */
    uint8_t bit = (from>to)?0:1;
    uint8_t CMD1[] = { MADCTL, madctl & ~(bit<<4) };
    SPI_Transmit(sizeof(CMD1), CMD1);

    uint8_t CMD[] = { PTLAR, (WIDTH-to+XSTART) >> 8, WIDTH-to+XSTART,
                           (WIDTH-from+XSTART) >> 8, WIDTH-from+XSTART };
    SPI_Transmit(sizeof(CMD), CMD);

    /* partial mode on */
    uint8_t CMD2[] = { PTLON };
    SPI_Transmit(sizeof(CMD2), CMD2);
}

void Delay(uint32_t d) {
    _Delay(d);
}

void Backlight_Pct(uint8_t p) {
        Pin_BLK_Pct(p % 101);
}
