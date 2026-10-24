#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void SPI_MasterInit();

void SPI_EnableSlave(int slave_bit, int slave_port);
void SPI_DisableSlave(int slave_bit, int slave_port);

uint8_t SPI_Transmit(char cData);
void SPI_Read(uint8_t slave_bit, uint8_t slave_port, uint8_t cmd, uint8_t *buffer, int size);

#endif // SPI_H
