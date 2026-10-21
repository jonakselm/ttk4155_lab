#include "pch.h"
#include "spi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "io_defines.h"

void SPI_MasterInit(void)
{
    DDRB |= (1 << PB_MOSI) | (1 << PB_SCK) | (1 << PB_DISPLAY_SS);
    PORTB &= ~(1 << PB_DISPLAY_SS);
    DDRB &= ~(1 << PB_MISO);
    /* Enable SPI, Master, set clock rate fck/4, MSB first, rising edge */
    SPCR = (1 << SPE) | (0 << DORD) | (1 << MSTR) | (0 << CPOL) | (0 << SPR0);
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
