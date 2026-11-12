#include "sam.h"
#include "ir.h"

void irInit(void)
{
    // ADC clock (larger than dword, (next - sizeof (DWORD)))
    PMC->PMC_PCER1 = 1u << (ID_ADC - 32);

    ADC->ADC_CR = ADC_CR_SWRST;

    // ADC clock = MCK / ((PRESCAL + 1) * 2) = 84 MHz / 6 = 14 MHz (max 22 MHz)
    ADC->ADC_MR = ADC_MR_FREERUN_ON
                | ADC_MR_PRESCAL(2)
                | ADC_MR_STARTUP_SUT64
                | ADC_MR_SETTLING_AST3
                | ADC_MR_TRACKTIM(15)
                | ADC_MR_TRANSFER(2);

    // Enabling the channel also switches PB17 over to the analog input
    // (next - sizeof (DWORD))
    ADC->ADC_CHER = 1u << IR_ADC_CHANNEL;
    ADC->ADC_CR   = ADC_CR_START;
}

uint16_t irRead(void)
{
    return ADC->ADC_CDR[IR_ADC_CHANNEL] & ADC_CDR_DATA_Msk;
}
