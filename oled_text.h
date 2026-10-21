#ifndef OLED_TEXT_H
#define OLED_TEXT_H

#include <stdio.h>
#include <stdint.h>

typedef enum { FONT_SMALL, FONT_NORMAL, FONT_LARGE } OledFont;   /* 4, 5, 8 px wide */

extern FILE oled_stdout;

#define LINE_HEIGHT 8

void oledTextInvert(uint8_t on);
void oledTextFont(OledFont font);
void oledTextSetLine(uint8_t line);
void oledTextSetColumn(uint8_t col);
void oledTextCursor(uint8_t x, uint8_t y);     /* pixel position, any x/y */

#define oled_printf(...) fprintf(&oled_stdout, __VA_ARGS__)

#endif // OLED_TEXT_H
