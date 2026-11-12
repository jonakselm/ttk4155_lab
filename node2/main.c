#define F_CPU 84000000
#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h"
#include "can.h"
#include "gamepad.h"
#include "encoder.h"
#include <time.h>
#include "pid.h"
#include "motor.h"
#include "servo.h"
#include "ir.h"
#include "input.h"

/*
 * Remember to update the Makefile with the (relative) path to the uart.c file.
 * This starter code will not compile until the UART file has been included in the Makefile. 
 * If you get somewhat cryptic errors referencing functions such as _sbrk, 
 * _close_r, _write_r, _fstat etc, you have most likely not done that correctly.

 * If you get errors such as "arm-none-eabi-gcc: no such file", you may need to reinstall the arm gcc packages using
 * apt or your favorite package manager.
 */

#define EVER ;;

#define ENC_LEFT    0
#define ENC_RIGHT   5611

#define KP             4.0f
#define KI             0.0f
#define KD             0.3f
#define OUT_MAX        192.0f
#define MIN_MOVE       32.0f

#define CONTROL_PERIOD_MS 10

#define IR_THRESHOLD  2048

#define MAX_RATE     5000.0f
#define END_MARGIN    300
#define MAX_LEAD      500

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    uart_init(F_CPU, 9600);
    printf("Uart initialized\r\n");

    //////////////
    // Motor: PWM channel 0 on D20, direction on D7 (also turns on the PWM clock)
    motorInit();

    // Servo: PWM channel 1 on D21, 20 ms period, no pulse until a game starts
    servoInit();
    //////////////

    // Set D6/PC24 output
    PIOC->PIO_PER = PIO_PER_P22;
    PIOC->PIO_OER = PIO_OER_P22;

    // Set D9/PC21 to input
    PIOC->PIO_PER = PIO_PER_P21;


    CanInit canInit;
    canInit.brp = 41;
    canInit.propag = 5;
    canInit.phase1 = 4;
    canInit.phase2 = 3;
    canInit.sjw = 0;
    canInit.smp = 0;
    can_init(canInit, 0);

    initEncoder();
    irInit();                           // IR receiver on A8
    uint64_t nextEncoderPrint = 0;

    // Position controller (PI): moves the carriage to the setpoint.
    // The joystick moves the setpoint.
    // Input: encoder counts over the track. Output: motor -OUT_MAX..OUT_MAX.
    // kp = 1 gives full power for an error of the whole track.
    Pid motorPid;
    pidInit(&motorPid, KP, KI, KD,
            ENC_LEFT, ENC_RIGHT,
            -OUT_MAX, OUT_MAX, 200.0f);

    uint64_t lastControl = time_now();
    float   target   = readEncoder();   // the setpoint, in encoder counts
    int16_t motorOut = 0;
    uint8_t irWasBlocked = 0;           // beam state last update
    uint8_t gameActive   = 0;           // 0 until node 1 sends START, 0 again after a goal


    GamepadState gps = { .sliderActive = 0, .lastSlider = 0,
        .servoFromTouch = 0, .lastJoyY = 128, .lastTouchX = 0,
        .haveGp = 0 };

    unsigned int c = 0;
    irInit();

    for (EVER)
    {
        CanMsg msg;
        //can_rx(&msg);
        // printf("PRintfLOOOOOOOP\r\n");
        int ir = irRead();
        // printf("IR: %i\r\n", ir);


        if (can_rx(&msg))
        {
            if (msg.id == CAN_ID_START)
            {
                gameActive = 1;
            }
            if (msg.id == CAN_ID_GOAL)
            {
                gameActive = 0;
            }
            else if (msg.id == CAN_ID_GAMEPAD && gameActive)
            {
                Gamepad gp = canDecodeGamepadData(&msg);
                handleGamepadInput(&gp, &gps);
            }
        }
        c = 0;

        if (time_now() >= nextEncoderPrint)
        {
            nextEncoderPrint = time_now() + msecs(500);
            // printf("pos %ld  target %ld  out %d  joyX %u  gp %d\r\n",
            //        (long)readEncoder(), (long)target, motorOut, lastGp.joyX, haveGp);
        }

        // Position controller, at a steady rate for PID
        uint64_t now = time_now();
        if (now - lastControl >= msecs(CONTROL_PERIOD_MS))
        {
            float dt = totalSeconds(now - lastControl);
            lastControl = now;
            float pos = readEncoder();

            if (!gps.haveGp || !gameActive)
            {
                motorOut = 0;
                pidReset(&motorPid);
                target = pos;
            }
            else
            {
                if (gps.sliderActive)
                {
                    // Slider: absolute position. The setpoint moves towards where
                    // the slider points, at most MAX_RATE, so it doesn't jump
                    float wanted = ENC_LEFT + (ENC_RIGHT - ENC_LEFT) * (gps.lastSlider / 255.0f);
                    float step = MAX_RATE * dt;
                    if (wanted > target + step)
                        target += step;
                    else if (wanted < target - step)
                        target -= step;
                    else
                        target = wanted;
                }
                else
                {
                    // Joystick X -> how fast the setpoint moves (counts per second):
                    // 0 inside the deadzone, then a straight line up to +-MAX_RATE
                    float rate = 0.0f;
                    int16_t offset = (int16_t)gps.lastGp.joyX - JOY_CENTER;
                    if (offset > JOY_DEADZONE || offset < -JOY_DEADZONE)
                    {
                        int16_t dist = (offset < 0) ? -offset : offset;
                        float x = (float)(dist - JOY_DEADZONE) / (128 - JOY_DEADZONE);
                        if (x > 1.0f)
                            x = 1.0f;
                        rate = (offset < 0) ? -x * MAX_RATE : x * MAX_RATE;
                    }

                    // The joystick increases/decreases the setpoint; centred = it stays put
                    target += rate * dt;
                }

                // Not too far from the carriage, so it can't run away
                if (target > pos + MAX_LEAD) target = pos + MAX_LEAD;
                if (target < pos - MAX_LEAD) target = pos - MAX_LEAD;

                // Not into the ends of the track
                if (target > ENC_RIGHT - END_MARGIN) target = ENC_RIGHT - END_MARGIN;
                if (target < ENC_LEFT  + END_MARGIN) target = ENC_LEFT  + END_MARGIN;

                motorOut = (int16_t)pidUpdate(&motorPid, target, pos, dt);
                // if (rate != 0.0f && motorOut != 0)       // only while the stick is pushed
                // {
                //     if (motorOut > 0 && motorOut <  MIN_MOVE) motorOut =  MIN_MOVE;
                //     if (motorOut < 0 && motorOut > -MIN_MOVE) motorOut = -MIN_MOVE;
                // }
            }
            motorSet(motorOut);

            // Goal: send once, at the moment the beam becomes blocked
            uint8_t irBlocked = irRead() < IR_THRESHOLD;
            if (irBlocked && !irWasBlocked && gameActive)
            {
                // Game over: stop everything right away, then tell node 1
                gameActive = 0;
                motorSet(0);
                servoOff();
                PIOC->PIO_SODR = PIO_SODR_P22;  // solenoid off

                CanMsg goal = { .id = CAN_ID_GOAL, .length = 1, .byte = { 1 } };
                can_tx(goal);
            }
            irWasBlocked = irBlocked;
        }
    }
}
