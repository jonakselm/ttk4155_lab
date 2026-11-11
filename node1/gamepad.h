#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "common.h"
#include <stdint.h>
#include "can.h"

typedef struct
{
    // uint8_t deadzoneMinX, deadzoneMaxX;
    // uint8_t deadzoneMinY, deadzoneMaxY;
    uint8_t joyMinX, joyMaxX;
    uint8_t joyMinY, joyMaxY;
    // uint8_t touchMinX, touchMaxX;
    // uint8_t touchMinY, touchMaxY;
    uint8_t centerX, centerY;
} CalibrationData;

void gamepadInit();

char valueToPercentage(uint8_t value);

void calibrate(volatile char *address, CalibrationData *cd);
void calculateCalibrated(CalibrationData *cd, Gamepad *gp);

void readGamepad(volatile char *address, Gamepad *gp);
void readGamepadSPI(Gamepad *gp);

void readCalibrated(volatile char *address, CalibrationData *cd, Gamepad *gp);
void readCalibratedSPI(CalibrationData *cd, Gamepad *gp);

CanMessage canEncodeGamepadData(Gamepad *gp);

#endif // GAMEPAD_H
