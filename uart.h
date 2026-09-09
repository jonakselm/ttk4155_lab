#ifndef UART_H
#define UART_H

void uart_init( unsigned int ubrr );

void uart_transmit( unsigned char data );

unsigned char uart_receive( void );

void uart_flush( void );

void uart_print(char *str);

void uart_link_printf();

#endif // UART_H
