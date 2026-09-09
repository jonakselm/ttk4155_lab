#define F_CPU 4915200
#define BAUD 9600
#define UBRR ((F_CPU / BAUD / 16) - 1)
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include <stdio.h>

#define EVER ;;

int main()
{
    DDRB = 1;
    PORTB = 1;
    uart_init(UBRR);
    uart_link_printf();

    int c = 0;
    for (EVER)
    {
        // uart_transmit('c');
        // uart_print("Det funke!\r\n");
        unsigned char r = uart_receive();
        _delay_ms(10);
        // printf("%c %i\n\r", r, c);
        // _delay_ms(10);
        // unsigned char r = uart_receive();
        // _delay_ms(10);
        // uart_transmit(r);
        // _delay_ms(10);
        PORTB ^= 1;
        c++;
    }
}
