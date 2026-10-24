#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <stdint.h>

typedef struct __attribute__((packed))
{
    union
    {
        uint8_t right;
        struct
        {
            uint8_t R1:1;
            uint8_t R2:1;
            uint8_t R3:1;
            uint8_t R4:1;
            uint8_t R5:1;
            uint8_t R6:1;
        };
    };
    union
    {
        uint8_t left;
        struct
        {
            uint8_t L1:1;
            uint8_t L2:1;
            uint8_t L3:1;
            uint8_t L4:1;
            uint8_t L5:1;
            uint8_t L6:1;
            uint8_t L7:1;
        };
    };
    union
    {
        uint8_t nav;
        struct
        {
            uint8_t NB:1;
            uint8_t NR:1;
            uint8_t ND:1;
            uint8_t NL:1;
            uint8_t NU:1;
        };
    };
} Buttons;

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

typedef struct
{
    uint8_t joyX, joyY;
    uint8_t touchX, touchY;
    Buttons buttons;
    uint8_t slider;
} Gamepad;

void gamepadInit();

char valueToPercentage(uint8_t value);

void calibrate(volatile char *address, CalibrationData *cd);
void calculateCalibrated(CalibrationData *cd, Gamepad *gp);

void readGamepad(volatile char *address, Gamepad *gp);
void readGamepadSPI(Gamepad *gp);

void readCalibrated(volatile char *address, CalibrationData *cd, Gamepad *gp);
void readCalibratedSPI(CalibrationData *cd, Gamepad *gp);

#endif // GAMEPAD_H
