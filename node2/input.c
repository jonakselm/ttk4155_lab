#include "input.h"
#include "servo.h"
#include "sam.h"

void handleGamepadInput(Gamepad *gp, GamepadState *gps)
{
    if (!gps->haveGp)
    {
        gps->lastSlider = gp->slider;
        gps->lastJoyY   = gp->joyY;
        gps->lastTouchX = gp->touchX;
    }
    gps->lastGp = *gp;
    gps->haveGp = 1;

    // Motor: whichever was used last, slider or joystick X
    int16_t sliderChange = (int16_t)gp->slider - gps->lastSlider;
    if (sliderChange >= JOY_DEADZONE || sliderChange <= -JOY_DEADZONE)
    {
        gps->lastSlider   = gp->slider;
        gps->sliderActive = 1;
    }
    int16_t joyXOffset = (int16_t)gp->joyX - JOY_CENTER;
    if (joyXOffset > JOY_DEADZONE || joyXOffset < -JOY_DEADZONE)
        gps->sliderActive = 0;           // joystick pushed: joystick takes over

    // Servo: whichever changed last, joystick Y or touch X
    int16_t joyYChange   = (int16_t)gp->joyY   - gps->lastJoyY;
    int16_t touchXChange = (int16_t)gp->touchX - gps->lastTouchX;
    if (joyYChange >= JOY_DEADZONE || joyYChange <= -JOY_DEADZONE)
    {
        gps->lastJoyY       = gp->joyY;
        gps->servoFromTouch = 0;
    }
    if (touchXChange >= JOY_DEADZONE || touchXChange <= -JOY_DEADZONE)
    {
        gps->lastTouchX     = gp->touchX;
        gps->servoFromTouch = 1;
    }
    servoSetJoystick(gps->servoFromTouch ? gps->lastTouchX : gps->lastJoyY);

    // printf("joy: %3u %3u  touch: %3u %3u  slider: %3u  R: %02X  L: %02X  nav: %02X\r\n",
    //    gp.joyX, gp.joyY, gp.touchX, gp.touchY, gp.slider,
    //    gp.buttons.right, gp.buttons.left, gp.buttons.nav);

    if (gp->buttons.R3)
        PIOC->PIO_CODR = PIO_CODR_P22;
    else
        PIOC->PIO_SODR = PIO_SODR_P22;
}
