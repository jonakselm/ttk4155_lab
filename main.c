#define F_CPU 4915200
#define BAUD 9600
#define UBRR ((F_CPU / BAUD / 16) - 1)
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include <stdio.h>
#include "sram_test.h"
#include "helpers.h"
#include "gamepad.h"

#define EVER ;;

enum ADC { ADC_A0, ADC_A1, ADC_A2, ADC_PD,
    ADC_INH, ADC_BIP, ADC_DIFF, ADC_ALL };

int main()
{
    DDRB = 1;
    PORTB = 1;
    uart_init(UBRR);
    uart_link_printf();
    printf("\n\nUART initialized\n");
    _delay_ms(1);
    
    // Enable external SRAM/XMEM
    MCUCR |= 1 << SRE;
    // Disable high-order bits in use by JTAG
    SFIOR = 1 << XMM2;

    //////////
    // Timer OC1A setup
    DDRD |= (1 << 5);
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << WGM11) | (0 << WGM10);
    TCCR1B = (1 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);

    ICR1 = 2;
    OCR1A = 1;
    // End Timer setup
    //////////CH1

    Gamepad gp;
    volatile char *adc_mmio = (char *) 0x1000; 
    CalibrationData cd;
    calibrate(adc_mmio, &cd);

    for (EVER)
    {
        readCalibrated(adc_mmio, &cd, &gp);
        printf("ADC:    %3i, %3i, %3i, %3i\r",
                gp.joyX, gp.joyY, gp.touchX, gp.touchY);
        // printf("ADC:    %3i%%, %3i%%, %3i%%, %3i%%\r", valueToPercentage(gp.joyX),
        //         valueToPercentage(gp.joyY), valueToPercentage(gp.touchX),
        //         valueToPercentage(gp.touchY));
    }
}
