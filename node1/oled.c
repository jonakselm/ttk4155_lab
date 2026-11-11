#include "pch.h"
#include "oled.h"
#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"
#include "io_defines.h"

void oledWriteCmd(uint8_t cmd)
{
    PORT_DISPLAY_DC &= ~(1 << BIT_DISPLAY_DC);
    SPI_Transmit(cmd);
}

void oledWriteData(uint8_t data)
{
    PORT_DISPLAY_DC |= (1 << BIT_DISPLAY_DC);
    SPI_Transmit(data);
}

void oledReset(void)
{
    PORT_DISPLAY_RESET |= (1 << BIT_DISPLAY_RESET);
    _delay_ms(1);
    PORT_DISPLAY_RESET &= ~(1 << BIT_DISPLAY_RESET);
    _delay_us(10);
    PORT_DISPLAY_RESET |= (1 << BIT_DISPLAY_RESET);
    _delay_ms(1);
}

void oledInit(void)
{
    // Set D/C, DISP_SS and DISP_RESET to output
    DDR_DISPLAY_DC |= 1 << BIT_DISPLAY_DC;
    DDR_DISPLAY_SS |= 1 << BIT_DISPLAY_SS;
    DDR_DISPLAY_RESET |= (1 << BIT_DISPLAY_RESET);

    SPI_EnableSlave(BIT_DISPLAY_SS, &PORT_DISPLAY_SS);


    oledReset();

    oledWriteCmd(0xAE);     // Display OFF
    oledWriteCmd(0xD5);     // Clock divide / oscillator
    oledWriteCmd(0x70);
    oledWriteCmd(0xA8);     // Multiplex ratio
    oledWriteCmd(0x3F);     //   64
    oledWriteCmd(0xD3);     // Display offset
    oledWriteCmd(0x00);
    oledWriteCmd(0x40);     // Start line 0
    oledWriteCmd(0xA1);     // Segment remap (0xA0 if mirrored)
    oledWriteCmd(0xC8);     // COM scan remapped (0xC0 if upside down)
    oledWriteCmd(0xDA);     // COM pins config
    oledWriteCmd(0x12);
    oledWriteCmd(0x81);     // Contrast
    oledWriteCmd(0x7F);
    oledWriteCmd(0xD9);     // Pre-charge period
    oledWriteCmd(0x22);
    oledWriteCmd(0xDB);     // VCOMH deselect level
    oledWriteCmd(0x34);
    oledWriteCmd(0x20);     // Memory addressing mode
    oledWriteCmd(0x00);     //   horizontal
    oledWriteCmd(0xA4);     // Display follows RAM
    oledWriteCmd(0xA6);     // Normal (not inverted)
    oledWriteCmd(0xAF);     // Display ON
    _delay_ms(100);                 // SEG/COM on after ~100 ms

    SPI_DisableSlave(BIT_DISPLAY_SS, &PORT_DISPLAY_SS);
}

void oledGotoLine(uint8_t line)
{
    oledWriteCmd(0x22);
    oledWriteCmd(line & OLED_PAGES);
    oledWriteCmd((line + 1) & OLED_PAGES);
}

void oledGotoColumn(uint8_t col)
{
    oledWriteCmd(0x21);
    oledWriteCmd(col & OLED_WIDTH);
    oledWriteCmd((col + 1) & OLED_WIDTH);
}

// Set write window to the whole screen (columns 0-127, pages 0-7)
void oledSetFullWindow(void)
{
    oledWriteCmd(0x21);
    oledWriteCmd(0);
    oledWriteCmd(OLED_WIDTH - 1);
    oledWriteCmd(0x22);
    oledWriteCmd(0);
    oledWriteCmd(OLED_PAGES - 1);
}

void oledFill(uint8_t pattern)
{
    oledSetFullWindow();
    for (uint16_t i = 0; i < OLED_WIDTH * OLED_PAGES; i++)
        oledWriteData(pattern);
}

void oledCheckerboard(void)
{
    oledSetFullWindow();
    for (uint8_t page = 0; page < OLED_PAGES; page++)
        for (uint8_t col = 0; col < OLED_WIDTH; col++)
            oledWriteData((((col >> 3) ^ page) & 1) ? 0xFF : 0x00);
}
