#include "sam.h"
#include "servo.h"
#include "common.h"

// 20 ms = 50 Hz
#define SERVO_PERIOD_US 20000           

static int16_t lastJoy = -1000;         /* last joystick value used; -1000 = none yet,
                                           so the next value always goes through */

void servoInit(void)
{
    // PWM clock (PWM is peripheral 36)
    PMC->PMC_PCER1 = PMC_PCER1_PID36;

    // Clock A = MCK / 84 = 1 MHz, so the numbers below are microseconds
    PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);

    // Channel 1: clock A, CDTY = high time (CPOL)
    PWM->PWM_CH_NUM[1].PWM_CMR  = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[1].PWM_CPRD = PWM_CPRD_CPRD(SERVO_PERIOD_US);
    PWM->PWM_CH_NUM[1].PWM_CDTY = PWM_CDTY_CDTY(0);   // no pulse until the first servoSet*() call

    // D21/PB13 to the PWM (peripheral B, PWMH1)
    PIOB->PIO_ABSR |= PIO_ABSR_P13;
    PIOB->PIO_PDR   = PIO_PDR_P13;

    PWM->PWM_ENA = PWM_ENA_CHID1;
}

void servoSetUs(uint32_t us)
{
    // Never outside the servo's safe range (manual 3.7.2)
    if (us < SERVO_MIN_US) us = SERVO_MIN_US;
    if (us > SERVO_MAX_US) us = SERVO_MAX_US;
    PWM->PWM_CH_NUM[1].PWM_CDTYUPD = us;    // applied at the next period
}

void servoOff(void)
{
    // CDTY = 0 with CPOL set keeps the output low: no pulse at all
    PWM->PWM_CH_NUM[1].PWM_CDTYUPD = 0;
    lastJoy = -1000;                    // next joystick value always goes through
}

void servoSetJoystick(uint8_t value)
{
    // Ignore small jumps in the joystick value: only move the servo when the
    // stick has moved more than JOY_DEADZONE steps since the last update
    int16_t change = (int16_t)value - lastJoy;
    if (change > -JOY_DEADZONE && change < JOY_DEADZONE)
        return;
    lastJoy = value;

    servoSetUs(SERVO_MIN_US + (uint32_t)value * (SERVO_MAX_US - SERVO_MIN_US) / 255u);
}
