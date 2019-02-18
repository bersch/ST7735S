ST7735S
=======

an OS independent library for the neat ST7735S 80x160 IPS 0.96" TFT-Module

# Features

 * MIT License
 * OS independent
 * Implements: Pixel, fillScreen, Line, Circle, Color, Fonts
 * BDF-Font converter included (bdf2c.pl) 
 * super fast with buffered writes

Adapt this driver to your desired Platform by editng the st7735s_compat.c file.
With the adapted file this driver may be used for: Arduino, PIC, ARM, Teensy,
Raspberry Pi, compatibles like Banana Pi or Orange Pi and many others
(for Big-Endian-Platforms the color setting shuould be modified).

Buffered writes speeds up everything. However if you use Arduino on 
Atmega328 compile with -DBUFFER=0 to reduce memory usage.
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

