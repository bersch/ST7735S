ST7735S
=======

an OS independent library for the neat ST7735S 80x160 IPS 0.96" TFT-Module

# Features

 * MIT License
 * OS independent
 * Implements: Pixel, fillScreen, Line, Circle, Color, Fonts
 * BDF-Font converter included (bdf2c.pl) 
 * super fast with buffered writes
 * great improvement on low mem with HVBUFFER

Adapt this driver to your desired Platform by editng the st7735s_compat.c file.
With the adapted file this driver may be used for: Arduino, PIC, ARM, Teensy,
Raspberry Pi, compatibles like Banana Pi or Orange Pi and many others
(for Big-Endian-Platforms the color setting shuould be modified).

Buffered writes speeds up everything. Implemented are three buffer modes:

 * BUFFER1  - actually no buffered writes, slow, for limited ram.
 * HVBUFFER - one line buffer with WIDTH * 2 bytes size. Acts as for either
    row or column buffer. 
 * BUFFER   - full frame WIDTH * HEIGHT * 2 bytes size. very fast.

The HVBUFFER is based on the fact, that due to design of the ST7735 to write
a single pixel one need to write 13 bytes on the SPI bus. For two bytes that are
adjacent in horizonal or vertical direction one need to write 15 bytes.
HVBUFFER stores one pixel and checks if the following pixel is adjacent.
Depending on the next pixel HVBUFFER acts like a row or column cache as long
as following pixels are adjacent. If the next pixel is not adjacent, the
cached pixels are flushed. Therefore the last action needs to be flushBuffer().

However if you use Arduino on  Atmega328 compile with -DBUFFER=0 to 
reduce memory usage. Perhaps HVBUFFER will work as well.
With a STM32H743 are 90/s full buffer updates possible.

This library uses the Terminus Font available at
http://terminus-font.sourceforge.net/

The display size is bigger than the tagged size of 80x160, mine is 106x161 pixels.
You may modify st7735s.h and change WIDTH and HEIGHT to meet your requirements.

In order to use your desired BDF-fonts use the ./tools/bdf2c.pl script.
The bdf2c.pl script can only handle fonts with fixed width.

The chars of a font can be limited by defining a range '0-9'.

# Wiring

   MODULE                   MCU 
   ------                   ---
           \              /
    GND    |--------------| GND
           |              |
    VCC    |--------------| VCC
           |              |
    SCL    |<-------------| SCLK (SPI Clock)
           |              |
    SDA    |<-------------| MOSI SPI (Master Out / Slave In)
           |              | 
    RES    |<-------------| Reset
           |              |
    DC     |<-------------| Data/Command
           |              |
    CS     |<-------------| CS (Chip Select)
           |              |
    BLK    |<-------------| Backlight (PWM)
           |              |
           /              \
           

 * Resources

 Datasheet: (https://www.crystalfontz.com/controllers/Sitronix/ST7735S/320)

