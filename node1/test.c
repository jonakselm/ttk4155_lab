/*
 * oled_spi_test.c
 *
 * Basic bring-up test for an SSD1309 128x64 OLED on an ATmega162,
 * using the SSD1309 4-wire SPI interface (BS[2:0] = 000).
 *
 * Wiring (ATmega162 -> SSD1309):
 *   PB7 (SCK)   -> D0  (SCLK)
 *   PB5 (MOSI)  -> D1  (SDIN)
 *   PB3         -> CS#
 *   PB2         -> D/C#   (change OLED_DC_* below if needed)
 *   PB1         -> RES#   (change OLED_RES_* below if needed)
 *   PB4 (SS) is not used for the OLED, but is kept as an output (driven
 *   high) so the SPI stays in master mode.
 *   D2 left open, D3..D7, E, R/W# tied to GND (SSD1309 ds. 8.1.3)
 *
 * Note: SSD1309 logic is VDD = 1.65..3.3 V. If the AVR runs at 5 V,
 * make sure your OLED module has level shifting / 5 V tolerant inputs.
 *
 * Serial mode is write-only on the SSD1309, so this is a *visual* test:
 * each step shows a known pattern for a couple of seconds.
 *
 *   1. Entire display ON (A5h)      -> every pixel lit, ignores RAM
 *   2. Clear RAM                    -> fully dark
 *   3. Fill RAM with 0xFF           -> fully lit (tests RAM writes)
 *   4. Checkerboard                 -> 8x8 squares
 *   5. Border + page stripes        -> checks orientation / edges
 *   6. Inverse display (A7h)        -> previous pattern inverted
 *   7. Contrast sweep 0x00..0xFF    -> brightness ramps up
 *   then loops forever.
 *
 * Build:
 *   avr-gcc -mmcu=atmega162 -DF_CPU=4915200UL -Os -o oled_spi_test.elf oled_spi_test.c
 *   avr-objcopy -O ihex oled_spi_test.elf oled_spi_test.hex
 */

#ifndef F_CPU
#define F_CPU 4915200UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* ---------- Pin configuration ---------- */
#define OLED_CS_DDR    DDRB
#define OLED_CS_PORT   PORTB
#define OLED_CS_PIN    PB3

#define OLED_DC_DDR    DDRB
#define OLED_DC_PORT   PORTB
#define OLED_DC_PIN    PB2

#define OLED_RES_DDR   DDRD
#define OLED_RES_PORT  PORTD
#define OLED_RES_PIN   PD3

#define OLED_WIDTH     128
#define OLED_PAGES     8        /* 64 rows / 8 bits per page */

#define CS_LOW()    (OLED_CS_PORT  &= ~(1 << OLED_CS_PIN))
#define CS_HIGH()   (OLED_CS_PORT  |=  (1 << OLED_CS_PIN))
#define DC_CMD()    (OLED_DC_PORT  &= ~(1 << OLED_DC_PIN))
#define DC_DATA()   (OLED_DC_PORT  |=  (1 << OLED_DC_PIN))
#define RES_LOW()   (OLED_RES_PORT &= ~(1 << OLED_RES_PIN))
#define RES_HIGH()  (OLED_RES_PORT |=  (1 << OLED_RES_PIN))

/* ---------- SPI (ATmega162 hardware SPI) ---------- */
static void spi_init(void)
{
    /* MOSI, SCK as outputs. MISO is input (unused, SSD1309 serial is write-only).
     * PB4 (SS) must be an output (or an input held high), otherwise a low level
     * on it forces the SPI out of master mode. It is driven high here. */
    PORTB |= (1 << PB4);
    DDRB  |= (1 << PB5) | (1 << PB7) | (1 << PB4);

    OLED_CS_DDR |= (1 << OLED_CS_PIN);
    CS_HIGH();

    /* Enable SPI, master, MSB first, mode 0 (SSD1309 samples SDIN on rising SCLK).
     * SCK = F_CPU/4 (SPR1:0 = 00, SPI2X = 0). The SSD1309 allows tcycle >= 100 ns
     * (10 MHz), so even F_CPU = 16 MHz -> 4 MHz SCK is well within spec. */
    SPCR = (1 << SPE) | (1 << MSTR);
    SPSR &= ~(1 << SPI2X);
}

static void spi_write(uint8_t b)
{
    SPDR = b;
    while (!(SPSR & (1 << SPIF)))
        ;
}

/* ---------- SSD1309 low level ---------- */
/* D/C# is sampled on the 8th SCLK of each byte, so it is set before the
 * transfer and only changed after SPIF signals the byte is done. */
static void oled_cmd(uint8_t c)
{
    DC_CMD();
    CS_LOW();
    spi_write(c);
    CS_HIGH();
}

static void oled_cmd2(uint8_t c, uint8_t arg)
{
    DC_CMD();
    CS_LOW();
    spi_write(c);
    spi_write(arg);
    CS_HIGH();
}

static void oled_reset(void)
{
    OLED_DC_DDR  |= (1 << OLED_DC_PIN);
    OLED_RES_DDR |= (1 << OLED_RES_PIN);

    RES_HIGH();
    _delay_ms(1);
    RES_LOW();          /* datasheet: RES# low >= 3 us */
    _delay_us(10);
    RES_HIGH();
    _delay_ms(1);
}

static void oled_init(void)
{
    oled_reset();

    oled_cmd(0xAE);             /* Display OFF */
    oled_cmd2(0xD5, 0x70);      /* Clock divide ratio = 1, Fosc = reset value */
    oled_cmd2(0xA8, 0x3F);      /* Multiplex ratio = 64 */
    oled_cmd2(0xD3, 0x00);      /* Display offset = 0 */
    oled_cmd(0x40);             /* Display start line = 0 */
    oled_cmd(0xA1);             /* Segment remap: column 127 -> SEG0 (flip if mirrored) */
    oled_cmd(0xC8);             /* COM scan direction remapped (flip with 0xC0 if upside down) */
    oled_cmd2(0xDA, 0x12);      /* COM pins: alternative config */
    oled_cmd2(0x81, 0x7F);      /* Contrast */
    oled_cmd2(0xD9, 0x22);      /* Pre-charge: phase1 = 2, phase2 = 2 DCLK */
    oled_cmd2(0xDB, 0x34);      /* VCOMH ~0.78 x VCC */
    oled_cmd2(0x20, 0x00);      /* Horizontal addressing mode */
    oled_cmd(0xA4);             /* Display follows RAM */
    oled_cmd(0xA6);             /* Normal (not inverted) */
    oled_cmd(0xAF);             /* Display ON */
    _delay_ms(100);             /* SEG/COM on after ~100 ms (tAF) */
}

/* Set the write window to the whole screen and leave CS low, D/C high,
 * ready for 1024 data bytes. */
static void oled_begin_frame(void)
{
    DC_CMD();
    CS_LOW();
    spi_write(0x21); spi_write(0);  spi_write(OLED_WIDTH - 1);  /* columns 0..127 */
    spi_write(0x22); spi_write(0);  spi_write(OLED_PAGES - 1);  /* pages   0..7   */
    DC_DATA();
}

static void oled_end_frame(void)
{
    CS_HIGH();
}

static void oled_fill(uint8_t pattern)
{
    oled_begin_frame();
    for (uint16_t i = 0; i < (uint16_t)OLED_WIDTH * OLED_PAGES; i++)
        spi_write(pattern);
    oled_end_frame();
}

/* 8x8 checkerboard: each byte is one column of 8 pixels in a page. */
static void oled_checkerboard(void)
{
    oled_begin_frame();
    for (uint8_t page = 0; page < OLED_PAGES; page++)
        for (uint8_t col = 0; col < OLED_WIDTH; col++)
            spi_write((((col >> 3) ^ page) & 1) ? 0xFF : 0x00);
    oled_end_frame();
}

/* 1-pixel border around the screen plus a small filled block in the
 * top-left corner, so you can see if the image is mirrored/flipped. */
static void oled_border(void)
{
    oled_begin_frame();
    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        for (uint8_t col = 0; col < OLED_WIDTH; col++) {
            uint8_t b = 0;
            if (col == 0 || col == OLED_WIDTH - 1) b = 0xFF;   /* left/right edge */
            if (page == 0)               b |= 0x01;            /* top row   (bit 0 of page 0) */
            if (page == OLED_PAGES - 1)  b |= 0x80;            /* bottom row (bit 7 of page 7) */
            if (page == 0 && col >= 4 && col < 12) b |= 0x3C;  /* marker: top-left corner */
            spi_write(b);
        }
    }
    oled_end_frame();
}

int main(void)
{
    spi_init();
    oled_init();

    for (;;) {
        /* 1. All pixels on regardless of RAM: verifies SPI + command path + panel power */
        oled_cmd(0xA5);
        _delay_ms(2000);
        oled_cmd(0xA4);

        /* 2. Clear */
        oled_fill(0x00);
        _delay_ms(1000);

        /* 3. Full white from RAM: verifies data writes (D/C# high) */
        oled_fill(0xFF);
        _delay_ms(2000);

        /* 4. Checkerboard */
        oled_checkerboard();
        _delay_ms(2000);

        /* 5. Border + top-left marker */
        oled_border();
        _delay_ms(3000);

        /* 6. Inverse display */
        oled_cmd(0xA7);
        _delay_ms(2000);
        oled_cmd(0xA6);

        /* 7. Contrast sweep on a full-white screen */
        oled_fill(0xFF);
        for (uint16_t c = 0; c <= 0xFF; c += 5) {
            oled_cmd2(0x81, (uint8_t)c);
            _delay_ms(20);
        }
        oled_cmd2(0x81, 0x7F);
        _delay_ms(1000);
    }
}
