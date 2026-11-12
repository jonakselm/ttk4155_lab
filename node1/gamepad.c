#include "gamepad.h"
#include "framebuffer.h"
#define F_CPU 4915200
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "helpers.h"
#include <stdlib.h>
#include <stdint.h>
#include "oled_text.h"
#include "io_defines.h"
#include "spi.h"
#include "can.h"

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

void gamepadInit()
{
    DDR_IO_SS |= 1 << BIT_IO_SS;
    PORT_IO_SS |= 1 << BIT_IO_SS;
}

char valueToPercentage(uint8_t value)
{
    return ((char)(value * 0.3922) - 50) << 1;
}

/* Is the stick where calibration step 'index' wants it?
 * 0 = left, 1 = right, 2 = down, 3 = up, 4 = centre */
int hasMoved(Gamepad *gp, int index)
{
    switch (index)
    {
    case 0: return gp->joyX < 60;
    case 1: return gp->joyX > 190;
    case 2: return gp->joyY < 60;
    case 3: return gp->joyY > 190;
    case 4: return gp->joyX > 100 && gp->joyX < 155 &&
                   gp->joyY > 100 && gp->joyY < 155;
    }
    return 0;
}

void calibrateSPI(CalibrationData *cd)
{
    static const char *const prompt[5] = {
        "Push LEFT!\n", "Push RIGHT!\n", "Push DOWN!\n", "Push UP!\n", "Let go!\n"
    };
    Gamepad gp;

    cd->valid = 0;
    for (int i = 0; i < 5; i++)
    {
        oledFbClear();
        oledTextCursor(0, 0);
        oled_printf("%s", prompt[i]);
        oledFbFlush();

        /* Wait until the stick is where this step wants it */
        do
        {
            readGamepadSPI(&gp);
        } while (!hasMoved(&gp, i));
        _delay_ms(200);

        oledFbClear();
        oledTextCursor(0, 0);
        oled_printf("Hold it!\n");
        oledFbFlush();

        /* Average the readings taken while the stick is still in
         * position: noise spikes barely matter, and readings after
         * letting go are skipped. SPI reads are slow, so 2000 is short. */
        uint32_t sumX = 0, sumY = 0;
        uint16_t count = 0;
        for (unsigned int j = 0; j < 2000u; j++)
        {
            readGamepadSPI(&gp);
            if (hasMoved(&gp, i))
            {
                sumX += gp.joyX;
                sumY += gp.joyY;
                count++;
            }
        }
        if (count == 0)
            count = 1;              /* let go at once: values become 0, so valid fails */
        uint8_t avgX = sumX / count;
        uint8_t avgY = sumY / count;

        oledFbClear();
        oledTextCursor(0, 0);
        switch (i)
        {
        case 0:
            cd->joyMinX = avgX;
            oled_printf("X min is: %3i\n", cd->joyMinX);
            break;
        case 1:
            cd->joyMaxX = avgX;
            oled_printf("X max is: %3i\n", cd->joyMaxX);
            break;
        case 2:
            cd->joyMinY = avgY;
            oled_printf("Y min is: %3i\n", cd->joyMinY);
            break;
        case 3:
            cd->joyMaxY = avgY;
            oled_printf("Y max is: %3i\n", cd->joyMaxY);
            break;
        case 4:
            cd->centerX = avgX;
            cd->centerY = avgY;
            oled_printf("Center is: %3i, %3i\n\n", cd->centerX, cd->centerY);
            break;
        }
        oledFbFlush();
        _delay_ms(500);
    }

    /* Only use it if min < centre < max on both axes,
     * otherwise calculateCalibrated() could divide by zero */
    cd->valid = cd->joyMinX < cd->centerX && cd->centerX < cd->joyMaxX &&
                cd->joyMinY < cd->centerY && cd->centerY < cd->joyMaxY;

    oledFbClear();
    oledTextCursor(0, 0);
    oled_printf("X is:      %i - %i\n", cd->joyMinX, cd->joyMaxX);
    oled_printf("Y is:      %i - %i\n", cd->joyMinY, cd->joyMaxY);
    oled_printf("Center is: %i,  %i\n", cd->centerX, cd->centerY);
    oled_printf(cd->valid ? "OK\n" : "FAILED, try again\n");
    oledFbFlush();
    _delay_ms(5000);
}

void calculateCalibrated(CalibrationData *cd, Gamepad *gp)
{
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

void readGamepad(volatile char *address, Gamepad *gp)
{
    address[0] = 0;
    while (!(PIN_BUSY & (1 << BIT_BUSY)));

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

void readGamepadSPI(Gamepad *gp)
{
    uint8_t buf[3];

    // Read joystick
    SPI_Read(BIT_IO_SS, &PORT_IO_SS, 0x03, buf, 3); 
    gp->joyX = buf[0];
    gp->joyY = buf[1];

    // Read touchpad
    SPI_Read(BIT_IO_SS, &PORT_IO_SS, 0x01, buf, 3);
    gp->touchX = buf[0];
    gp->touchY = buf[1];

    // Buttons
    SPI_Read(BIT_IO_SS, &PORT_IO_SS, 0x04, buf, 3);
    gp->buttons.right = buf[0];
    gp->buttons.left = buf[1];
    gp->buttons.nav = buf[2];

    // Slider
    SPI_Read(BIT_IO_SS, &PORT_IO_SS, 0x02, buf, 2);
    gp->slider = buf[0];

    _delay_us(2);
}

void readCalibrated(volatile char *address, CalibrationData *cd, Gamepad *gp)
{
    readGamepad(address, gp);
    calculateCalibrated(cd, gp);
}

void readCalibratedSPI(CalibrationData *cd, Gamepad *gp)
{
    readGamepadSPI(gp);
    calculateCalibrated(cd, gp);
}

CanMessage canEncodeGamepadData(Gamepad *gp)
{
    CanMessage msg;
    msg.id = CAN_ID_GAMEPAD;
    msg.dword[0] = gp->dword[0];
    msg.dword[1] = gp->dword[1];
    msg.length = 8;

    return msg;
}
