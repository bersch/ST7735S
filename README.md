ST7735S
=======

a portable library for the neat ST7735S 80x160 IPS 0.96" TFT-Display

### Features

 * MIT License
 * portable
 * Implements: Pixel, fillScreen, Line, Rect, filledRect, Circle, Arc, Pie, Color, Fonts
 * BDF-Font converter included (bdf2c.pl) 
 * super fast with buffered writes
 * great improvement on low mem with HVBUFFER


### Architecture

```
+-----+---------------+
|     |   GFX/Fonts   | gfx.c, fonts.c
+     +---------------+
|       Driver        | st7735s.c
+---------------------+
|   SPI Compat Layer  | st7735s_compat.c
+---------------------+
|        OS-API       | Arduino/STM32/...
+---------------------+
```

Adapt this driver to your desired Platform by editng the `st7735s_compat.c` file.

Buffered writes speeds up everything. Implemented are three buffer modes:

 * `BUFFER1`  - actually no buffered writes, slow, for limited ram.
 * `HVBUFFER` - one line buffer with WIDTH * 2 bytes size. Acts as for either
    row or column buffer. 
 * `BUFFER`   - full frame WIDTH * HEIGHT * 2 bytes size. very fast.

The `HVBUFFER` is based on the fact, that due to design of the ST7735 to write
a single pixel one need to write 13 bytes on the SPI bus. For two bytes that are
adjacent in horizonal or vertical direction one need to write 15 bytes.
`HVBUFFER` stores one pixel and checks if the following pixel is adjacent.
Depending on the next pixel `HVBUFFER` acts like a row or column cache as long
as following pixels are adjacent. If the next pixel is not adjacent, the
cached pixels are flushed. Therefore the last action needs to be `flushBuffer()`.

However if you use Arduino on  Atmega328 compile with `-DBUFFER1` to 
reduce memory usage. Perhaps `HVBUFFER` will work as well.

This library uses the Terminus Font available at
http://terminus-font.sourceforge.net/

In order to use your desired BDF-fonts use the `./tools/bdf2c.pl` script.
The bdf2c.pl script can only handle fonts with fixed width.

The chars of a font can be limited by defining a range i.e. '0-9'.

### Quick Steps

Copy `st7735s_compat.h.default`  and `st7735s_compat.cpp.arduino` to your project dir and rename them to `.h` and `.cpp`. Edit those files and modify them for your needs. Link to ST7735S library.
You'll need to insert also includes to your code
```
#include "st7735s.h"
#include "fonts.h"
#include "gfx.h"
```


### Wiring

```
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
```

 * Resources

 Datasheet: (https://www.crystalfontz.com/controllers/Sitronix/ST7735S/320)

