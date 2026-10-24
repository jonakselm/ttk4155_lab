#include "pch.h"
#include "spi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "io_defines.h"

void SPI_MasterInit(void)
{
    DDRB |= (1 << BIT_MOSI) | (1 << BIT_SCK) | (1 << BIT_SS);
    PORTB |= (1 << BIT_SS);
    // DDRB |= (1 << PB_IO_SS);
    DDRB &= ~(1 << BIT_MISO);
    /* Enable SPI, Master, set clock rate fck/4, MSB first, rising edge */
    SPCR = (1 << SPE) | (0 << DORD) | (1 << MSTR) | (0 << CPOL) | (0 << SPR0);
}

void SPI_EnableSlave(int slave_bit, int slave_port)
{
    slave_port &= ~(1 << slave_bit);
}

void SPI_DisableSlave(int slave_bit, int slave_port)
{
    slave_port |= (1 << slave_bit);
}

uint8_t SPI_Transmit(char cData)
{
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while(!(SPSR & (1 << SPIF)));

    if (!(SPCR & (1 << MSTR)))
        printf("Master mode lost\n");

    return SPDR;
}

void SPI_Read(uint8_t slave_bit, uint8_t slave_port, uint8_t cmd, uint8_t *buffer, int size)
{
    SPI_EnableSlave(slave_bit, slave_port);
    SPI_Transmit(cmd);
    _delay_us(40);
    for (int i = 0; i < size; i++)
    {
        buffer[i] = SPI_Transmit(0x00);
        _delay_us(2);
    }
    SPI_DisableSlave(slave_bit, slave_port);
}
