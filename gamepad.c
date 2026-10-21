#include "framebuffer.h"
#define F_CPU 4915200
#include "gamepad.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "helpers.h"
#include <stdlib.h>
#include <stdint.h>
#include "oled_text.h"
#include "io_defines.h"

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

char valueToPercentage(uint8_t value)
{
    return ((char)(value * 0.3922) - 50) << 1;
}

int hasMoved(Gamepad *gp, Gamepad *prevGp, int index)
{
    int movedThreshold = 100;
    // printf("GP:         %3i, %3i, %3i, %3i\n",
    //         gp->joyX, gp->joyY, gp->touchX, gp->touchY);
    // printf("Prev GP:    %3i, %3i, %3i, %3i\n\n",
    //         prevGp->joyX, prevGp->joyY, prevGp->touchX, prevGp->touchY);

    switch (index)
    {
    case 0:
    case 1:
        oledFbClear();
        oled_printf("%i\r", abs((char)gp->joyX - (char)prevGp->joyX));
        oledFbFlush();
        return abs((char)gp->joyX - (char)prevGp->joyX) > movedThreshold;
        break;
    case 2:
    case 3:
        oled_printf("%i\r", abs((char)gp->joyY - (char)prevGp->joyY));
        return abs((char)gp->joyY - (char)prevGp->joyY) > movedThreshold;
        break;
    case 4:
        // return  abs((char)gp->joyX - (char)prevGp->joyX) > movedThreshold >> 2 &&
        //     abs((char)gp->joyY - (char)prevGp->joyY) > movedThreshold >> 2;
        return 1;
        break;
    }
    return 0;
}

void calibrate(volatile char *address, CalibrationData *cd)
{
    Gamepad prevGp;
    Gamepad gp;
    readGamepad(address, &gp);
    for (int i = 0; i < 5; i++)
    {
        oledFbClear();
        oledTextCursor(0, 0);
        oled_printf("Move!\n");
        oledFbFlush();
        do
        {
            prevGp = gp;
            readGamepad(address, &gp);
        } while (!hasMoved(&gp, &prevGp, i));
        _delay_ms(200);

        oledFbClear();
        oledTextCursor(0, 0);
        oled_printf("Hold it!\n");
        oledFbFlush();
        for (unsigned int j = 0; j < 50000u; j++)
        {
            readGamepad(address, &gp);
        }
        
        oledFbClear();
        oledTextCursor(0, 0);
        switch (i)
        {
        case 0:
            cd->joyMinX = gp.joyX;
            oled_printf("X min is: %3i\n", cd->joyMinX);
            break;
        case 1:
            cd->joyMaxX = gp.joyX;
            oled_printf("X max is: %3i\n", cd->joyMaxX);
            break;
        case 2:
            cd->joyMinY = gp.joyY;
            oled_printf("Y min is: %3i\n", cd->joyMinY);
            break;
        case 3:
            cd->joyMaxY = gp.joyY;
            oled_printf("Y max is: %3i\n", cd->joyMaxY);
            break;
        case 4:
            cd->centerX = gp.joyX;
            cd->centerY = gp.joyY;
            oled_printf("Center is: %3i, %3i\n\n", cd->centerX, cd->centerY);
            break;
        }
        oledFbFlush();
        _delay_ms(500);
    }
    oledFbClear();
    oledTextCursor(0, 0);
    oled_printf("X is:      %i - %i\n", cd->joyMinX, cd->joyMaxX);
    oled_printf("Y is:      %i - %i\n", cd->joyMinY, cd->joyMaxY);
    oled_printf("Center is: %i,  %i\n", cd->centerX, cd->centerY);
    oledFbFlush();
    _delay_ms(5000);
}

int lowpass(uint16_t value, uint16_t prevValue, float alpha)
{
    return alpha * value + (1 - alpha) * prevValue;
}

void readGamepad(volatile char *address, Gamepad *gp)
{
    address[0] = 0;
    while (!(PINB & (1 << PB_BUSY)));

    float alpha = 0.8;

    gp->joyX = address[0];
    gp->joyY = address[0];
    gp->touchX = address[0];
    gp->touchY = address[0];

    // gp->joyX = lowpass(address[0], gp->joyX, alpha);
    // gp->joyY = lowpass(address[0], gp->joyY, alpha);
    // gp->touchX = lowpass(address[0], gp->touchX, alpha);
    // gp->touchY = lowpass(address[0], gp->touchY, alpha);
}

void readCalibrated(volatile char *address, CalibrationData *cd, Gamepad *gp)
{
    readGamepad(address, gp);

    int halfpoint = 127;

    int negRangeX = cd->centerX - cd->joyMinX;
    // float stepNegX = halfpoint / negRangeX;

    int posRangeX = cd->joyMaxX - cd->centerX;
    // float stepPosX = halfpoint / posRangeX;


    int negRangeY = cd->centerY - cd->joyMinY;
    // float stepNegY = halfpoint / negRangeY;

    int posRangeY = cd->joyMaxY - cd->centerY;
    // float stepPosY = halfpoint / posRangeY;


    int16_t tmp;
    tmp = (int16_t)gp->joyX - cd->centerX;
    // printf("\t\t\t\t\t%i\n", tmp);
    if (tmp > 0)
    {
        // printf("\n\nHalf range %i\n", posRangeX);
        // printf("Step size %f\n\n", stepPosX);
        gp->joyX = min((int16_t)128 + tmp * halfpoint / posRangeX, 255);
    }
    else
    {
        // printf("Half range %i\n", negRangeX);
        // printf("Step size %f\n\n", stepNegX);
        gp->joyX = max((int16_t)128 + tmp * halfpoint / negRangeX, 0);
    }

    tmp = (int16_t)gp->joyY - cd->centerY;
    // printf("\t\t\t\t\t%i\t%i\t%i\n", gp->joyY, cd->centerY, tmp);
    if (tmp > 0)
    {
        gp->joyY = min((int16_t)128 + tmp * halfpoint / posRangeY, 255);
    }
    else
    {
        gp->joyY = max((int16_t)128 + tmp * halfpoint / negRangeY, 0);
    }
}
