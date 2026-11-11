/*
 * mcp2515.h - low-level driver for the MCP2515 CAN controller (SPI).
 * Values from the MCP2515 datasheet (DS21801): instructions in table 12-1,
 * register addresses in table 11-1, register bits in sections 3, 4 and 10.
 */
#ifndef MCP2515_H
#define MCP2515_H

#include <stdint.h>

/* SPI instructions (table 12-1) */
typedef enum {
    MCP_WRITE        = 0x02,    /* 0000 0010 */
    MCP_READ         = 0x03,    /* 0000 0011 */
    MCP_BIT_MODIFY   = 0x05,    /* 0000 0101 */
    MCP_RTS_TX0      = 0x81,    /* 1000 0001 */
    MCP_RTS_TX1      = 0x82,    /* 1000 0010 */
    MCP_RTS_TX2      = 0x84,    /* 1000 0100 */
    MCP_READ_STATUS  = 0xA0,    /* 1010 0000 */
    MCP_RESET        = 0xC0,    /* 1100 0000 */
} Mcp2515Instruction;

/* Register addresses (table 11-1) */
typedef enum {
    MCP_CANSTAT      = 0x0E,
    MCP_CANCTRL      = 0x0F,
    MCP_TEC          = 0x1C,
    MCP_REC          = 0x1D,
    MCP_CNF3         = 0x28,
    MCP_CNF2         = 0x29,
    MCP_CNF1         = 0x2A,
    MCP_CANINTE      = 0x2B,
    MCP_CANINTF      = 0x2C,
    MCP_EFLG         = 0x2D,
    MCP_TXB0CTRL     = 0x30,
    MCP_TXB0SIDH     = 0x31,
    MCP_TXB0SIDL     = 0x32,
    MCP_TXB0DLC      = 0x35,
    MCP_TXB0D0       = 0x36,
    MCP_RXB0CTRL     = 0x60,
    MCP_RXB0SIDH     = 0x61,
    MCP_RXB0SIDL     = 0x62,
    MCP_RXB0DLC      = 0x65,
    MCP_RXB0D0       = 0x66,
    MCP_RXB1CTRL     = 0x70,
    MCP_RXB1SIDH     = 0x71,
} Mcp2515Register;

/* Operating modes: REQOP bits in CANCTRL / OPMOD bits in CANSTAT (register 10-1) */
typedef enum {
    MODE_NORMAL      = 0x00,    /* 000x xxxx */
    MODE_SLEEP       = 0x20,    /* 001x xxxx */
    MODE_LOOPBACK    = 0x40,    /* 010x xxxx */
    MODE_LISTENONLY  = 0x60,    /* 011x xxxx */
    MODE_CONFIG      = 0x80,    /* 100x xxxx */
    MODE_MASK        = 0xE0,    /* bits 7-5 */
} Mcp2515Mode;

/* Register bits */
typedef enum {
    MCP_TXREQ        = 0x08,    /* TXB0CTRL bit 3: message pending (register 3-1) */
    MCP_RX0IF        = 0x01,    /* CANINTF/CANINTE bit 0: RXB0 full */
    MCP_RX1IF        = 0x02,    /* CANINTF/CANINTE bit 1: RXB1 full */
    MCP_TX0IF        = 0x04,    /* CANINTF/CANINTE bit 2: TXB0 empty */
} Mcp2515Bit;

void    mcp2515_init(void);                                    /* CS pin + reset */
void    mcp2515_reset(void);
uint8_t mcp2515_read(Mcp2515Register address);
void    mcp2515_read_many(Mcp2515Register address, uint8_t *buf, uint8_t n);
void    mcp2515_write(Mcp2515Register address, uint8_t data);
void    mcp2515_write_many(Mcp2515Register address, const uint8_t *buf, uint8_t n);
void    mcp2515_request_to_send(Mcp2515Instruction rts);       /* MCP_RTS_TX0 .. TX2 */
void    mcp2515_bit_modify(Mcp2515Register address, uint8_t mask, uint8_t data);
uint8_t mcp2515_read_status(void);

#endif
