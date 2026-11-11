/* mcp2515.c - every function is one SPI instruction from datasheet section 12:
 * CS low, instruction byte, (address), data, CS high. */
#include "pch.h"
#include <util/delay.h>
#include "mcp2515.h"
#include "spi.h"
#include "io_defines.h"
#include "stdio.h"

static inline void cs_low(void)  { SPI_EnableSlave(BIT_CAN_SS, &PORT_CAN_SS); }
static inline void cs_high(void) { SPI_DisableSlave(BIT_CAN_SS, &PORT_CAN_SS); }

void mcp2515_init(void)
{
    PORT_CAN_SS |= (1 << BIT_CAN_SS);           /* CS high first ... */
    DDR_CAN_SS  |= (1 << BIT_CAN_SS);           /* ... then output */
    mcp2515_reset();
}

void mcp2515_reset(void)
{
    cs_low();
    SPI_Transmit(MCP_RESET);
    cs_high();
    _delay_ms(1);                               /* oscillator start-up after reset */
}

uint8_t mcp2515_read(Mcp2515Register address)
{
    cs_low();
    SPI_Transmit(MCP_READ);
    SPI_Transmit(address);
    uint8_t v = SPI_Transmit(0x00);             /* dummy byte clocks the value out */
    cs_high();
    return v;
}

void mcp2515_read_many(Mcp2515Register address, uint8_t *buf, uint8_t n)
{
    cs_low();
    SPI_Transmit(MCP_READ);
    SPI_Transmit(address);                      /* address auto-increments */
    for (uint8_t i = 0; i < n; i++)
        buf[i] = SPI_Transmit(0x00);
    cs_high();
}

void mcp2515_write(Mcp2515Register address, uint8_t data)
{
    cs_low();
    SPI_Transmit(MCP_WRITE);
    SPI_Transmit(address);
    SPI_Transmit(data);
    cs_high();
}

void mcp2515_write_many(Mcp2515Register address, const uint8_t *buf, uint8_t n)
{
    cs_low();
    SPI_Transmit(MCP_WRITE);
    SPI_Transmit(address);
    for (uint8_t i = 0; i < n; i++)
        SPI_Transmit(buf[i]);
    cs_high();
}

void mcp2515_request_to_send(Mcp2515Instruction rts_cmd)
{
    cs_low();
    SPI_Transmit(rts_cmd);
    cs_high();
}

void mcp2515_bit_modify(Mcp2515Register address, uint8_t mask, uint8_t data)
{
    cs_low();
    SPI_Transmit(MCP_BIT_MODIFY);
    SPI_Transmit(address);
    SPI_Transmit(mask);                         /* only bits set in mask change */
    SPI_Transmit(data);
    cs_high();
}

uint8_t mcp2515_read_status(void)
{
    cs_low();
    SPI_Transmit(MCP_READ_STATUS);
    uint8_t v = SPI_Transmit(0x00);
    cs_high();
    return v;
}
