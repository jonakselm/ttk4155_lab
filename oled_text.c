#include "oled_text.h"
#include "framebuffer.h"
#include "fonts.h"

static uint8_t cur_x, cur_y;
static uint8_t font_w = 5;
static const unsigned char *font_data = &font5[0][0];
static uint8_t inverted;

void oledTextInvert(uint8_t on)
{
    inverted = on;
}

void oledTextFont(OledFont font)
{
    switch (font)
    {
    case FONT_SMALL: font_w = 4; font_data = &font4[0][0]; break;
    case FONT_LARGE: font_w = 8; font_data = &font8[0][0]; break;
    default:         font_w = 5; font_data = &font5[0][0]; break;
    }
}

void oledTextSetLine(uint8_t line)
{
    cur_y = (line * LINE_HEIGHT) % OLED_HEIGHT;
    // oledTextCursor(cur_x, cur_y);
}

void oledTextSetColumn(uint8_t col)
{
    cur_x = (col % (int)(OLED_HEIGHT / font_w)) * font_w;
    // oledTextCursor(cur_x, cur_y);
}

void oledTextCursor(uint8_t x, uint8_t y)
{
    cur_x = x;
    cur_y = y;
}

static void draw_char(char c)
{
    if (c < ' ' || c > '~')
        c = '?';

    const unsigned char *glyph = font_data + (uint16_t)(c - ' ') * font_w;

    for (uint8_t col = 0; col < font_w; col++)
    {
        uint8_t bits = pgm_read_byte(glyph + col);
        for (uint8_t row = 0; row < 8; row++)
            oledFbPixel(cur_x + col, cur_y + row, ((bits >> row) & 1) ^ inverted);
            // oledFbPixel((cur_x + col) % OLED_WIDTH, (cur_y + row) % OLED_HEIGHT, bits & (1 << row));
    }
}

static int oled_putchar(char c, FILE *stream)
{
    (void)stream;

    if (c == '\n')
    {
        cur_x = 0;
        cur_y += 8;
        return 0;
    }
    if (c == '\r')
    {
        cur_x = 0;
        return 0;
    }

    uint8_t step = (font_w == 8) ? 8 : font_w + 1;
    if (cur_x + step > OLED_WIDTH)
    {
        cur_x = 0;
        cur_y += 8;
    }
    cur_y &= 0x3F;

    draw_char(c);
    for (uint8_t col = font_w; col < step; col++)
        for (uint8_t row = 0; row < 8; row++)
            oledFbPixel(cur_x + col, cur_y + row, inverted);
    cur_x += step;
    return 0;
}

FILE oled_stdout = FDEV_SETUP_STREAM(oled_putchar, NULL, _FDEV_SETUP_WRITE);
