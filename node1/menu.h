#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stddef.h>

typedef struct Menu Menu;

typedef struct {
    const char *label;
    void (*action)(void);
    Menu *submenu;
} MenuItem;

struct Menu {
    const char *title;
    const MenuItem *items;
    uint8_t count;
    Menu *parent;
};

typedef struct {
    Menu *current;
    uint8_t selected;
    uint8_t scroll;
} MenuState;

void menuInit(MenuState *s, Menu *root);
void menuDraw(MenuState *s);

void menuUp(MenuState *s);
void menuDown(MenuState *s);
void menuSelect(MenuState *s);
void menuBack(MenuState *s);

#endif
