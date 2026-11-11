#include "pch.h"           /* F_CPU, for _delay_ms */
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include "screensaver.h"
#include "oled.h"          /* oledWriteCmd() */
#include "framebuffer.h"
#include "oled_text.h"

#define LABEL "Noge"         /* text in the box, change to anything */

static int16_t x, y;        /* top-left corner of the box */
static int8_t  dx, dy;      /* direction: +1 or -1 */
static uint8_t box_w, box_h;
static uint8_t inverted;

void screensaverInit(void)
{
    box_w = strlen(LABEL) * 6 + 5;   /* 6 px per character + 3 px padding each side */
    box_h = 8 + 6;                   /* 8 px font + 3 px padding top and bottom */
    x = 17;
    y = 9;
    dx = 1;
    dy = 1;
    inverted = 0;
    oledTextFont(FONT_NORMAL);
    oledWriteCmd(0xA6);              /* start in normal (not inverted) mode */
}

void screensaverStep(void)
{
    uint8_t hit_x = 0, hit_y = 0;

    /* Move */
    x += dx;
    y += dy;

    /* Bounce off the left/right edges */
    if (x <= 0)                       { x = 0;                   dx = 1;  hit_x = 1; }
    else if (x + box_w >= OLED_WIDTH) { x = OLED_WIDTH - box_w;  dx = -1; hit_x = 1; }

    /* Bounce off the top/bottom edges */
    if (y <= 0)                        { y = 0;                   dy = 1;  hit_y = 1; }
    else if (y + box_h >= OLED_HEIGHT) { y = OLED_HEIGHT - box_h; dy = -1; hit_y = 1; }

    /* Every bounce "changes colour": invert the whole screen */
    if (hit_x || hit_y) {
        inverted = !inverted;
        // oledWriteCmd(inverted ? 0xA7 : 0xA6);
    }

    /* Draw the frame */
    oledFbClear();
    oledFbRect(x, y, box_w, box_h, 1);
    oledTextCursor(x + 3, y + 3);
    oled_printf(LABEL);
    oledFbFlush();

    /* Hit a corner exactly: celebrate with a flash */
    if (hit_x && hit_y) {
        for (uint8_t i = 0; i < 6; i++) {
            oledWriteCmd((i & 1) ? 0xA6 : 0xA7);
            _delay_ms(80);
        }
        oledWriteCmd(inverted ? 0xA7 : 0xA6);
    }
}
