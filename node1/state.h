#ifndef STATE_H
#define STATE_H

#include "menu.h"
#include "gamepad.h"

typedef enum {
    SCREEN_PING_PONG,
    SCREEN_MENU,
    SCREEN_SCREENSAVER,
    SCREEN_BUTTON_MAPPING,
    SCREEN_JOY_TEST,
    COUNT
} Screen;

/* Positions in the menu list in main.c - keep these in sync */
#define MENU_INDEX_PING_PONG         0
#define MENU_INDEX_SCREENSAVER       1
#define MENU_INDEX_CALIBRATE         2
#define MENU_INDEX_BUTTON_MAPPING    3
#define MENU_INDEX_JOY_TEST          4

/* Call once per loop, after readGamepadSPI() and before sending gp over CAN.
 * Runs whichever screen is showing. Once calibrated, gp is changed to
 * calibrated values. */
void stateUpdate(MenuState *menu, Gamepad *gp);

#endif // STATE_H
