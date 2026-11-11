#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void SPI_MasterInit();

typedef volatile uint8_t PORT_TYPE;

void SPI_EnableSlave(uint8_t slave_bit, PORT_TYPE *slave_port);
void SPI_DisableSlave(uint8_t slave_bit, PORT_TYPE *slave_port);

uint8_t SPI_Transmit(char cData);
void SPI_Read(uint8_t slave_bit, PORT_TYPE *slave_port, uint8_t cmd, uint8_t *buffer, int size);

#endif // SPI_H
