#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

#define OLED_WIDTH   128
#define OLED_HEIGHT  64
#define OLED_FB_SIZE (OLED_WIDTH * OLED_PAGES)   /* 1024 bytes */

/* Start of the framebuffer in external SRAM. Adjust to your memory map. */
#define OLED_FB_ADDR 0x1800

void oledFbClear(void);
void oledFbPixel(uint8_t x, uint8_t y, uint8_t on);
void oledFbHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t on);
void oledFbVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t on);
void oledFbRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on);
void oledFbFillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t on);
void oledFbFlush(void);

#endif // FRAMEBUFFER_H
