#ifndef INPUT_H
#define INPUT_H

#include "common.h"

#define JOY_CENTER    128

typedef struct 
{
    uint8_t sliderActive;
    uint8_t lastSlider;
    uint8_t servoFromTouch;
    uint8_t lastJoyY;
    uint8_t lastTouchX;
    int haveGp;
    Gamepad lastGp;
} GamepadState;

void handleGamepadInput(Gamepad *gp, GamepadState *gps);

#endif // INPUT_H
