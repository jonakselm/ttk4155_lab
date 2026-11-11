#define F_CPU 84000000
#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h"
#include "can.h"
#include <string.h>
#include "gamepad.h"
#include "io.h"
#include <time.h>

/*
 * Remember to update the Makefile with the (relative) path to the uart.c file.
 * This starter code will not compile until the UART file has been included in the Makefile. 
 * If you get somewhat cryptic errors referencing functions such as _sbrk, 
 * _close_r, _write_r, _fstat etc, you have most likely not done that correctly.

 * If you get errors such as "arm-none-eabi-gcc: no such file", you may need to reinstall the arm gcc packages using
 * apt or your favorite package manager.
 */

#define EVER ;;

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    uart_init(F_CPU, 9600);
    printf("Uart initialized\r\n");

    //////////////
    // Setup clock for PWM
    PMC->PMC_PCER1 |= PMC_PCDR1_PID36;

    // F_CPU / 1024, PWM D20/PB12
    PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPOL | PWM_CMR_CPRE_MCK_DIV_1024;
    PWM->PWM_CH_NUM[0].PWM_CDTY = PWM_CDTY_CDTY(0);
    PWM->PWM_CH_NUM[0].PWM_CPRD = PWM_CPRD_CPRD(255);
    PIOB->PIO_ABSR |= PIO_ABSR_P12;
    PIOB->PIO_PDR = PIO_PDR_P12;

    PWM->PWM_ENA |= PWM_ENA_CHID0;

    // F_CPU / 1024, PWM D21/PB13
    PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPOL | PWM_CMR_CPRE_MCK_DIV_1024;
    PWM->PWM_CH_NUM[1].PWM_CDTY = PWM_CDTY_CDTY(150);
    PWM->PWM_CH_NUM[1].PWM_CPRD = PWM_CPRD_CPRD(255);
    PIOB->PIO_ABSR |= PIO_ABSR_P13;
    PIOB->PIO_PDR = PIO_PDR_P13;

    // Enable channel
    PWM->PWM_ENA |= PWM_ENA_CHID1;
    // End PWM setup
    //////////////

    // Set D6/PC24 output
    PIOC->PIO_PER = PIO_PER_P24;
    PIOC->PIO_OER = PIO_PER_P24;


    CanInit canInit;
    canInit.brp = 41;
    canInit.propag = 5;
    canInit.phase1 = 4;
    canInit.phase2 = 3;
    canInit.sjw = 0;
    canInit.smp = 0;
    can_init(canInit, 0);

    unsigned int c = 0;

    for (EVER)
    {
        CanMsg msg;
        //can_rx(&msg);
        // printf("PRintfLOOOOOOOP\r\n");


        if (can_rx(&msg))
        {
            Gamepad gp = canDecodeGamepadData(&msg);

            printf("joy: %3u %3u  touch: %3u %3u  slider: %3u  R: %02X  L: %02X  nav: %02X\r\n",
               gp.joyX, gp.joyY, gp.touchX, gp.touchY, gp.slider,
               gp.buttons.right, gp.buttons.left, gp.buttons.nav);

            if (gp.buttons.R3)
                PIOC->PIO_CODR = PIO_CODR_P24;
            else
                PIOC->PIO_SODR = PIO_SODR_P24;
        }
        c = 0;
    }
}
