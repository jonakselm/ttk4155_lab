/* can.h - CAN driver on top of the MCP2515 driver. Same message type on node 2. */
#ifndef CAN_H
#define CAN_H

#include <stdint.h>

typedef struct {
    uint16_t id;        /* 11-bit standard identifier, 0..0x7FF */
    uint8_t  length;    /* 0..8 */
    union 
    {
        uint32_t dword[2];
        uint8_t  data[8];
    };
} CanMessage;

typedef enum {
    CAN_MODE_NORMAL   = 0x00,
    CAN_MODE_LOOPBACK = 0x40,   /* messages come back to us, nothing on the bus */
} CanMode;

uint8_t can_init(CanMode mode);           /* 0 = OK, 1 = MCP2515 not answering, 2 = mode change failed */
uint8_t can_send(const CanMessage *msg);  /* 0 = OK, 1 = previous message still pending */
uint8_t can_receive(CanMessage *msg);     /* 1 = message received, 0 = none */
uint8_t can_error_flags(void);            /* EFLG register, 0 = no errors */

#endif
