#include <stdint.h>
#include "state.h"
#include "screensaver.h"
#include "framebuffer.h"
#include "oled_text.h"
#include "can.h"

#define JOY_THRESH_UP    190     /* raw joystick thresholds, 0-255 */
#define JOY_THRESH_DOWN  60

static Screen screen = SCREEN_MENU;

static CalibrationData cd;      /* cd.valid = 0 until calibrated */

static uint8_t lastSelect, lastBack, lastUp, lastDown;

static uint8_t gameOver;        /* 1 after node 2 reports a goal */

void stateUpdate(MenuState *menu, Gamepad *gp)
{
    /* "Pressed" = held now but not last loop, so one press = one action */
    uint8_t selectHeld = gp->buttons.R4;
    uint8_t backHeld   = gp->buttons.L4;
    uint8_t upHeld     = gp->joyY > JOY_THRESH_UP;
    uint8_t downHeld   = gp->joyY < JOY_THRESH_DOWN;

    uint8_t selectPressed = selectHeld && !lastSelect;
    uint8_t backPressed   = backHeld && !lastBack;
    uint8_t upPressed     = upHeld && !lastUp;
    uint8_t downPressed   = downHeld && !lastDown;

    lastSelect = selectHeld;
    lastBack   = backHeld;
    lastUp     = upHeld;
    lastDown   = downHeld;

    switch (screen)
    {
    case SCREEN_PING_PONG:
        {
            // Node 2 says the ball broke the IR beam
            CanMessage rx;
            if (can_receive(&rx) && rx.id == CAN_ID_GOAL)
                gameOver = 1;

            oledFbClear();
            if (gameOver)
            {
                oledTextCursor((OLED_WIDTH - 9 * 6) / 2, OLED_HEIGHT / 2 - 4);   // 9 chars, 6 px each
                oled_printf("GAME OVER");
            }
            else
            {
                oledTextCursor(OLED_WIDTH / 2 - 14, OLED_HEIGHT / 2 - 4);
                oled_printf("Game!");

                CanMessage msg = canEncodeGamepadData(gp);
                can_send(&msg);
            }
            oledFbFlush();
        }
        break;
    case SCREEN_MENU:
        if (upPressed)
            menuUp(menu);
        if (downPressed)
            menuDown(menu);

        if (selectPressed)
        {
            if (menu->selected == MENU_INDEX_SCREENSAVER)
            {
                screensaverInit();
                screen = SCREEN_SCREENSAVER;
            }
            else if (menu->selected == MENU_INDEX_CALIBRATE)
            {
                calibrateSPI(&cd);         /* blocks until done */
            }
            else if (menu->selected == MENU_INDEX_JOY_TEST)
            {
                screen = SCREEN_JOY_TEST;
            }
            else if (menu->selected == MENU_INDEX_PING_PONG)
            {
                // New game: throw away old messages, so an old goal can't end it at once
                CanMessage old;
                while (can_receive(&old))
                    ;
                gameOver = 0;

                // Tell node 2 a new game starts: it turns the actuators on
                CanMessage start = { .id = CAN_ID_START, .length = 0 };
                can_send(&start);

                screen = SCREEN_PING_PONG;
            }
        }

        if (screen == SCREEN_MENU)
            menuDraw(menu);
        break;

    case SCREEN_SCREENSAVER:
        screensaverStep();
        break;

    case SCREEN_JOY_TEST:
    {
        /* Show the joystick as it will be sent: calibrated if possible */
        Gamepad shown = *gp;
        if (cd.valid)
            calculateCalibrated(&cd, &shown);

        oledFbClear();
        oledTextCursor(0, 0);
        oled_printf("Joystick %s\n\n", cd.valid ? "(calibrated)" : "(raw)");
        oled_printf("X: %3i\n", shown.joyX);
        oled_printf("Y: %3i\n", shown.joyY);
        oledFbFlush();
        break;
    }

    }

    if (backPressed && screen != SCREEN_MENU)
    {
        if (screen == SCREEN_PING_PONG)
        {
            CanMessage msg = { .id = CAN_ID_GOAL, .length = 1, .data = { 1 }};
            can_send(&msg);
        }
        screen = SCREEN_MENU;
    }

    /* Raw values are used above; calibrated values go out over CAN */
    if (cd.valid)
        calculateCalibrated(&cd, gp);
}
