#include "pch.h"
#include <util/delay.h>
#include "can.h"
#include "mcp2515.h"

uint8_t can_init(CanMode mode)
{
    mcp2515_init();                                     /* reset -> configuration mode */

    if ((mcp2515_read(MCP_CANSTAT) & MODE_MASK) != MODE_CONFIG)
        return 1;                                       /* no answer: check SPI / CS */

    /* Bit timing: 250 kbit/s with the MCP2515's 16 MHz crystal.
     * TQ = 2 * (BRP + 1) / 16 MHz = 250 ns, 16 TQ per bit:
     * SyncSeg 1 + PropSeg 6 + PS1 5 + PS2 4, sample point 75 %, SJW 1 TQ.
     * Node 2 must use the same timing. */
    mcp2515_write(MCP_CNF1, 3);                        /* SJW=0, BRP=41 */
    mcp2515_write(MCP_CNF2, (1 << 7) | (4 << 3) | 5);   /* BTLMODE=1, PHSEG1=4, PRSEG=5 */
    mcp2515_write(MCP_CNF3, 3);                         /* PHSEG2=3 */

    mcp2515_write(MCP_RXB0CTRL, 0x60);                  /* RXB0: accept every message */
    mcp2515_write(MCP_CANINTE, MCP_RX0IF);              /* INT pin low when RXB0 is full */

    mcp2515_bit_modify(MCP_CANCTRL, MODE_MASK, mode);   /* leave configuration mode */
    _delay_ms(1);
    if ((mcp2515_read(MCP_CANSTAT) & MODE_MASK) != mode)
        return 2;

    return 0;
}

uint8_t can_send(const CanMessage *msg)
{
    if (mcp2515_read(MCP_TXB0CTRL) & MCP_TXREQ)
        return 1;                                       /* TXB0 still busy */

    uint8_t len = msg->length > 8 ? 8 : msg->length;
    uint8_t frame[5 + 8] = {
        (uint8_t)(msg->id >> 3),                        /* SIDH: ID bits 10..3 */
        (uint8_t)((msg->id & 0x07) << 5),               /* SIDL: ID bits 2..0, standard frame */
        0, 0,                                           /* EID8, EID0 (unused) */
        len,                                            /* DLC */
    };
    for (uint8_t i = 0; i < len; i++)
        frame[5 + i] = msg->data[i];

    mcp2515_write_many(MCP_TXB0SIDH, frame, 5 + len);   /* registers 0x31.. are consecutive */
    mcp2515_request_to_send(MCP_RTS_TX0);
    return 0;
}

uint8_t can_receive(CanMessage *msg)
{
    if (!(mcp2515_read(MCP_CANINTF) & MCP_RX0IF))
        return 0;                                       /* nothing in RXB0 */

    uint8_t frame[5 + 8];
    mcp2515_read_many(MCP_RXB0SIDH, frame, sizeof frame);

    msg->id     = ((uint16_t)frame[0] << 3) | (frame[1] >> 5);
    msg->length = frame[4] & 0x0F;
    if (msg->length > 8) msg->length = 8;
    for (uint8_t i = 0; i < msg->length; i++)
        msg->data[i] = frame[5 + i];

    mcp2515_bit_modify(MCP_CANINTF, MCP_RX0IF, 0);      /* free RXB0 for the next message */
    return 1;
}

uint8_t can_error_flags(void)
{
    return mcp2515_read(MCP_EFLG);
}
