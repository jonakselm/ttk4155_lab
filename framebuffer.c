#include "framebuffer.h"
#include "oled.h"        /* oledWriteCmd(), oledWriteData() */
#include "io_defines.h"
#include "spi.h"

/* The framebuffer lives in external SRAM. XMEM must be enabled
 * (MCUCR |= 1 << SRE) before any of these functions are called. */
static uint8_t *const fb = (uint8_t *)OLED_FB_ADDR;

void oledFbClear(void)
{
    SPI_EnableSlave(BIT_DISPLAY_SS, PORT_DISPLAY_SS);
    for (uint16_t i = 0; i < OLED_FB_SIZE; i++)
        fb[i] = 0x00;
}

void oledFbPixel(uint8_t x, uint8_t y, uint8_t on)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
        return;                                  /* clip instead of wrapping */

    uint16_t index = (uint16_t)(y >> 3) * OLED_WIDTH + x;   /* page * 128 + x */
    uint8_t  mask  = 1 << (y & 7);                          /* bit within the page */

    if (on) fb[index] |=  mask;
    else    fb[index] &= ~mask;
}

void oledFbHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t on)
{
    for (uint16_t px = x; px < (uint16_t)x + w && px < OLED_WIDTH; px++)
        oledFbPixel(px, y, on);
}

void oledFbVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t on)
{
    for (uint16_t py = y; py < (uint16_t)y + h && py < OLED_HEIGHT; py++)
        oledFbPixel(x, py, on);
}

void oledFbRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on)
{
    if (w == 0 || h == 0)
        return;
    oledFbHLine(x, y,         w, on);   /* top    */
    oledFbHLine(x, y + h - 1, w, on);   /* bottom */
    oledFbVLine(x,         y, h, on);   /* left   */
    oledFbVLine(x + w - 1, y, h, on);   /* right  */
}

void oledFbFillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on)
{
    for (uint16_t py = y; py < (uint16_t)y + h && py < OLED_HEIGHT; py++)
        oledFbHLine(x, py, w, on);
}

/* Copy the whole framebuffer to the display. */
void oledFbFlush(void)
{
    oledWriteCmd(0x21);                 /* columns 0..127 */
    oledWriteCmd(0);
    oledWriteCmd(OLED_WIDTH - 1);
    oledWriteCmd(0x22);                 /* pages 0..7 */
    oledWriteCmd(0);
    oledWriteCmd(OLED_PAGES - 1);

    for (uint16_t i = 0; i < OLED_FB_SIZE; i++)
        oledWriteData(fb[i]);

    SPI_DisableSlave(BIT_DISPLAY_SS, PORT_DISPLAY_SS);
}
