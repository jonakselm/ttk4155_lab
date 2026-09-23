#define F_CPU 4915200
#define BAUD 9600
#define UBRR ((F_CPU / BAUD / 16) - 1)
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include <stdio.h>
#include "sram_test.h"
#include "helpers.h"

#define EVER ;;


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
    // Disable high-order bits for use by JTAG
    SFIOR = 1 << XMM2;

    volatile char *ext_ram = (char *) 0x1800; 

    // DDRE = 1;
    TCCR1B = (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);

    // unsigned char c = 0b00010001;
    int addr = 0;
    unsigned char c = 0;
    for (EVER)
    {
        SRAM_test();       
        // PORTB ^= 1;
        // ext_ram[addr] = c;
        // unsigned char ch = ext_ram[addr];
        // printf("Teller: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(c));
        // printf("SRAM:   "BYTE_TO_BINARY_PATTERN"\n\n", BYTE_TO_BINARY(ch));
        // _delay_ms(100);
        // c--;
    }
}
