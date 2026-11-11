#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stddef.h>     /* NULL */

typedef struct Menu Menu;

typedef struct {
    const char *label;          /* text shown in the menu */
    void (*action)(void);       /* called on select (NULL = none) */
    Menu *submenu;              /* opened on select (NULL = none) */
} MenuItem;

struct Menu {
    const char     *title;
    const MenuItem *items;
    uint8_t         count;
    Menu           *parent;     /* where "back" goes (NULL = top level) */
};

typedef struct {
    Menu   *current;            /* menu being shown */
    uint8_t selected;           /* index of highlighted item */
    uint8_t scroll;             /* index of first visible item */
} MenuState;

void menuInit(MenuState *s, Menu *root);
void menuDraw(MenuState *s);      /* draws into framebuffer and flushes */

void menuUp(MenuState *s);
void menuDown(MenuState *s);
void menuSelect(MenuState *s);
void menuBack(MenuState *s);

#endif
