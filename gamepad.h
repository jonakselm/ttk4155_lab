#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <stdint.h>

typedef struct
{
    uint8_t deadzoneMinX, deadzoneMaxX;
    uint8_t deadzoneMinY, deadzoneYMax;
    uint8_t joyMinX, joyMaxX;
    uint8_t joyMinY, joyMaxY;
    uint8_t touchMinX, touchMaxX;
    uint8_t touchMinY, touchMaxY;
    uint8_t centerX, centerY;
} CalibrationData;

typedef struct
{
    uint8_t joyX, joyY;
    uint8_t touchX, touchY;
} Gamepad;

char valueToPercentage(uint8_t value);

void calibrate(volatile char *address, CalibrationData *cd);

void readGamepad(volatile char *address, Gamepad *gp);

void readCalibrated(volatile char *address, CalibrationData *cd, Gamepad *gp);

#endif // GAMEPAD_H
