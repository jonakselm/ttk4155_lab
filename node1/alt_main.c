/* SPI mode test for the User IO-board.
 * For every SPI mode (0-3) and two clock speeds it:
 *   1. turns all six LEDs on (PWM 255), waits, turns them off
 *   2. reads the Info command (0x07) and prints it
 * Correct mode = LEDs light up AND Info prints a date like 2025-08-14T10:32:05.
 *
 * Build instead of main.c:  SOURCE_FILES := spi_mode_test.c uart.c
 * Self-contained: uses the SPI registers directly, not spi.c. */
#include "pch.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include "uart.h"

#define BAUD 9600
#define UBRR ((F_CPU / BAUD / 16) - 1)

/* ---- Pins: change if yours differ ---- */
#define IO_CS    PB4     /* IO board (header pin 2) */
#define DISP_CS  PB3     /* display, kept deselected */
#define CAN_CS   PD4     /* MCP2515, kept deselected (it also drives MISO) */

#define IO_SELECT()   (PORTB &= ~(1 << IO_CS))
#define IO_DESELECT() (PORTB |=  (1 << IO_CS))

static uint8_t xfer(uint8_t b)
{
    SPDR = b;
    while (!(SPSR & (1 << SPIF)))
        ;
    return SPDR;
}

static void io_write2(uint8_t cmd, uint8_t a, uint8_t b)
{
    IO_SELECT();
    _delay_us(40);                  /* settle after CS low */
    xfer(cmd);
    _delay_us(40);                  /* >= 40 us before first data byte */
    xfer(a);
    _delay_us(2);
    xfer(b);
    _delay_us(2);
    IO_DESELECT();
    _delay_us(50);
}

static void io_read(uint8_t cmd, uint8_t *buf, uint8_t n)
{
    IO_SELECT();
    _delay_us(40);
    xfer(cmd);
    _delay_us(40);
    for (uint8_t i = 0; i < n; i++) {
        buf[i] = xfer(0x00);
        _delay_us(5);               /* >= 2 us between data bytes */
    }
    IO_DESELECT();
    _delay_us(50);
}

static void all_leds(uint8_t width)
{
    for (uint8_t led = 0; led < 6; led++)
        io_write2(0x06, led, width);    /* LED PWM */
}

int main(void)
{
    uart_init(UBRR);
    uart_link_printf();
    printf("\n\n=== IO-board SPI mode test ===\n");

    /* All chip selects HIGH first, then outputs */
    PORTB |= (1 << IO_CS) | (1 << DISP_CS);
    DDRB  |= (1 << IO_CS) | (1 << DISP_CS) | (1 << PB5) | (1 << PB7);   /* + MOSI, SCK */
    DDRB  &= ~(1 << PB6);                                                /* MISO input */
    PORTD |= (1 << CAN_CS);
    DDRD  |= (1 << CAN_CS);

    static const uint8_t mode_bits[4] = {
        0,                              /* mode 0 */
        (1 << CPHA),                    /* mode 1 */
        (1 << CPOL),                    /* mode 2 */
        (1 << CPOL) | (1 << CPHA),      /* mode 3 */
    };
    static const uint8_t speed_bits[2] = { 0, (1 << SPR0) };   /* /64, /16 */
    static const char *speed_name[2]   = { "fck/4 (77 kHz)", "fck/16 (307 kHz)" };

    for (;;) {
        for (uint8_t s = 0; s < 2; s++) {
            for (uint8_t m = 0; m < 4; m++) {
                SPCR = (1 << SPE) | (1 << MSTR) | speed_bits[s] | mode_bits[m];
                // SPSR &= ~(0 << SPI2X);
                (void)SPSR; (void)SPDR;             /* clear a stale SPIF */

                printf("\nMode %u, %s, SPCR=%02X\n", m, speed_name[s], SPCR);
                printf("  LEDs ON  for 1.5 s ... ");
                all_leds(255);
                _delay_ms(1500);
                all_leds(0);
                printf("off\n");

                uint8_t info[19];
                io_read(0x07, info, sizeof info);
                printf("  Info: \"");
                for (uint8_t i = 0; i < sizeof info; i++)
                    putchar(info[i] >= ' ' && info[i] <= '~' ? info[i] : '.');
                printf("\"  hex:");
                for (uint8_t i = 0; i < 6; i++)
                    printf(" %02X", info[i]);
                printf(" ...\n");

                uint8_t joy[3];
                io_read(0x03, joy, 3);
                printf("  Joystick: X=%u Y=%u btn=%u\n", joy[0], joy[1], joy[2]);

                if (!(SPCR & (1 << MSTR)))
                    printf("  !! master mode lost (SS pin went low?)\n");

                _delay_ms(1000);
            }
        }
        printf("\n=== repeating ===\n");
    }
}
