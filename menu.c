#include <string.h>
#include "menu.h"
#include "framebuffer.h"
#include "oled_text.h"

#define DIVIDER_Y    11                 /* line under the title */
#define ITEMS_Y      14                 /* top of the first item row */
#define ROW_H        9                  /* 7 px text + 1 px above + 1 px below */
#define MENU_VISIBLE ((OLED_HEIGHT - ITEMS_Y) / ROW_H)   /* = 5 rows on screen */

void menuInit(MenuState *s, Menu *root)
{
    s->current  = root;
    s->selected = 0;
    s->scroll   = 0;
}

void menuDraw(MenuState *s)
{
    const Menu *m = s->current;

    /* Scroll so the selected item is always on screen.
     * Done here, so it works no matter how 'selected' was changed. */
    if (s->selected >= m->count)
        s->selected = m->count - 1;
    if (s->selected < s->scroll)
        s->scroll = s->selected;                          /* moved above the window */
    if (s->selected >= s->scroll + MENU_VISIBLE)
        s->scroll = s->selected - MENU_VISIBLE + 1;       /* moved below the window */

    oledFbClear();
    oledTextInvert(0);

    /* Centred title */
    oledTextFont(FONT_LARGE);
    uint8_t tw = strlen(m->title) * 8;
    oledTextCursor(tw < OLED_WIDTH ? (OLED_WIDTH - tw) / 2 : 0, 1);
    oled_printf("%s", m->title);
    oledFbHLine(0, DIVIDER_Y, OLED_WIDTH, 1);

    /* Visible items: scroll .. scroll + MENU_VISIBLE - 1 */
    oledTextFont(FONT_NORMAL);
    for (uint8_t i = 0; i < MENU_VISIBLE; i++) {
        uint8_t idx = s->scroll + i;
        if (idx >= m->count)
            break;

        uint8_t y   = ITEMS_Y + i * ROW_H;
        uint8_t sel = (idx == s->selected);

        if (sel)
            oledFbFillRect(0, y, OLED_WIDTH, ROW_H, 1);

        oledTextInvert(sel);
        oledTextCursor(6, y + 1);
        oled_printf("%s%s", m->items[idx].label, m->items[idx].submenu ? " >" : "");
        oledTextInvert(0);
    }

    oledFbFlush();
}

void menuUp(MenuState *s)
{
    if (s->selected > 0)
        s->selected--;
}

void menuDown(MenuState *s)
{
    if (s->selected + 1 < s->current->count)
        s->selected++;
}

void menuSelect(MenuState *s)
{
    const MenuItem *item = &s->current->items[s->selected];

    if (item->action)
        item->action();

    if (item->submenu) {
        s->current  = item->submenu;
        s->selected = 0;
        s->scroll   = 0;
    }
}

void menuBack(MenuState *s)
{
    if (s->current->parent) {
        s->current  = s->current->parent;
        s->selected = 0;
        s->scroll   = 0;
    }
}
