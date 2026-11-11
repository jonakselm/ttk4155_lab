#include "encoder.h"

void initEncoder(void)
{
    /* 1. Clock: TC2 channel 0 is called "TC6" in the PMC (ID 33).
     *    Note: ID_TC2 is channel 2 of TC0, not this one. (36.5.2) */
    PMC->PMC_PCER1 = 1u << (ID_TC6 - 32);

    /* 2. Pins: PC25 and PC26 to peripheral B, TIOA6 and TIOB6 (Table 36-4) */
    PIOC->PIO_ABSR |= PIO_PC25B_TIOA6 | PIO_PC26B_TIOB6;
    PIOC->PIO_PDR   = PIO_PC25B_TIOA6 | PIO_PC26B_TIOB6;

    /* 3. Channel 0: capture mode, counting the decoder output XC0,
     *    rising edge, TIOA as external trigger (36.6.14) */
    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0
                              | TC_CMR_ETRGEDG_RISING
                              | TC_CMR_ABETRG;

    /* 4. Quadrature decoder on, measuring position (36.7.14) */
    TC2->TC_BMR = TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA;

    /* 5. Start counting from 0 */
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

int32_t readEncoder(void)
{
    /* Counts up one way and down the other, so it can go negative */
    return (int32_t)TC2->TC_CHANNEL[0].TC_CV;
}
