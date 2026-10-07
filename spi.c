#include "pch.h"
#include "spi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define PB_SS 4
#define PB_MOSI 5
#define PB_MISO 6
#define PB_SCK 7

void SPI_MasterInit(void)
{
    DDRB |= (1 << PB_MOSI) | (1 << PB_SCK) | (1 << PB_SS);
    PORTB &= ~(1 << PB_SS);
    DDRB &= ~(1 << PB_MISO);
    /* Enable SPI, Master, set clock rate fck/16, MSB first, rising edge */
    SPCR = (1 << SPE) | (0 << DORD) | (1 << MSTR) | (0 << CPOL) | (1 << SPR0);
}
void SPI_MasterTransmit(char cData)
{
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while(!(SPSR & (1 << SPIF)));

    if (!(SPCR & (1 << MSTR)))
        printf("Master mode lost\n");
}
