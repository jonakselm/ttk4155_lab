#include "pch.h"
#include <stdio.h>
#include <util/delay.h>
#include "spi.h"
#include "can.h"

void can_loopback_test(void)
{
    SPI_MasterInit();
    printf("can_init: %u (0 = OK)\n", can_init(CAN_MODE_LOOPBACK));

    CanMessage tx = { .id = 0x123, .length = 5, .data = { 'T', 'E', 'S', 'T' } };
    CanMessage rx;

    for (uint8_t n = 0; n < 5; n++)
    {
        tx.data[4] = '0' + n;
        can_send(&tx);
        _delay_ms(10);
        if (can_receive(&rx))
            printf("got id=0x%03X len=%u \"%c%c%c%c%c%c\"\n",
                   rx.id, rx.length, rx.data[0], rx.data[1], rx.data[2], rx.data[3], rx.data[4], rx.data[5]);
        else
            printf("nothing received (EFLG=0x%02X)\n", can_error_flags());
    }
}
