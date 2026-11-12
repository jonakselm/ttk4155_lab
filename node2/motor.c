#include "sam.h"
#include <stdint.h>
#include "motor.h"

void motorInit(void)
{
    // PWM clock (PWM is peripheral 36)
    PMC->PMC_PCER1 = PMC_PCER1_PID36;

    // Channel 0: MCK/1024, period 255, CDTY = high time (CPOL), start stopped
    PWM->PWM_CH_NUM[0].PWM_CMR  = PWM_CMR_CPOL | PWM_CMR_CPRE_MCK_DIV_16;
    PWM->PWM_CH_NUM[0].PWM_CDTY = PWM_CDTY_CDTY(0);
    PWM->PWM_CH_NUM[0].PWM_CPRD = PWM_CPRD_CPRD(255);

    // D20/PB12 to the PWM (peripheral B, PWMH0)
    PIOB->PIO_ABSR |= PIO_ABSR_P12;
    PIOB->PIO_PDR   = PIO_PDR_P12;

    PWM->PWM_ENA = PWM_ENA_CHID0;

    // D7/PC23: direction, output
    PIOC->PIO_PER = PIO_PER_P23;
    PIOC->PIO_OER = PIO_OER_P23;
}

void motorSet(int16_t speed)
{
    if (MOTOR_DIR_INVERT)
        speed = -speed;

    if (speed >= 0)
        PIOC->PIO_SODR = PIO_SODR_P23;
    else
        PIOC->PIO_CODR = PIO_CODR_P23;

    uint32_t duty = (speed >= 0) ? speed : -speed;
    if (duty > 255)
        duty = 255;
    PWM->PWM_CH_NUM[0].PWM_CDTYUPD = duty;
}
