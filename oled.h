#ifndef OLED_H
#define OLED_H


#define OLED_WIDTH 128
#define OLED_PAGES 8

#include <stdint.h>

void oledWriteCmd(uint8_t cmd);

void oledWriteData(uint8_t data);

void oledReset(void);

void oledInit(void);

void oledGotoLine(uint8_t line);

void oledGotoColumn(uint8_t col);

void oledSetFullWindow(void);

void oledFill(uint8_t pattern);

void oledCheckerboard(void);

#endif // OLED_H
