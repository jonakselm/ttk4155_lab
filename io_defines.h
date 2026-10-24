#ifndef IO_DEFINES_H
#define IO_DEFINES_H

#include <avr/io.h>


/* ---------------- Port A ---------------- */

#define BIT_ADC0            0
#define PORT_ADC0           PORTA
#define DDR_ADC0            DDRA
#define PIN_ADC0            PINA

#define BIT_ADC1            1
#define PORT_ADC1           PORTA
#define DDR_ADC1            DDRA
#define PIN_ADC1            PINA

#define BIT_ADC2            2
#define PORT_ADC2           PORTA
#define DDR_ADC2            DDRA
#define PIN_ADC2            PINA

#define BIT_ADC3            3
#define PORT_ADC3           PORTA
#define DDR_ADC3            DDRA
#define PIN_ADC3            PINA

#define BIT_ADC4            4
#define PORT_ADC4           PORTA
#define DDR_ADC4            DDRA
#define PIN_ADC4            PINA

#define BIT_ADC5            5
#define PORT_ADC5           PORTA
#define DDR_ADC5            DDRA
#define PIN_ADC5            PINA

#define BIT_ADC6            6
#define PORT_ADC6           PORTA
#define DDR_ADC6            DDRA
#define PIN_ADC6            PINA

#define BIT_ADC7            7
#define PORT_ADC7           PORTA
#define DDR_ADC7            DDRA
#define PIN_ADC7            PINA


/* ---------------- Port B ---------------- */

#define BIT_CAN_SS          0
#define PORT_CAN_SS         PORTB
#define DDR_CAN_SS          DDRB
#define PIN_CAN_SS          PINB

#define BIT_BUSY            1
#define PORT_BUSY           PORTB
#define DDR_BUSY            DDRB
#define PIN_BUSY            PINB

#define BIT_DISPLAY_DC      2
#define PORT_DISPLAY_DC     PORTB
#define DDR_DISPLAY_DC      DDRB
#define PIN_DISPLAY_DC      PINB

#define BIT_DISPLAY_SS      3
#define PORT_DISPLAY_SS     PORTB
#define DDR_DISPLAY_SS      DDRB
#define PIN_DISPLAY_SS      PINB

#define BIT_SS              4
#define PORT_SS             PORTB
#define DDR_SS              DDRB
#define PIN_SS              PINB

#define BIT_MOSI            5
#define PORT_MOSI           PORTB
#define DDR_MOSI            DDRB
#define PIN_MOSI            PINB

#define BIT_MISO            6
#define PORT_MISO           PORTB
#define DDR_MISO            DDRB
#define PIN_MISO            PINB

#define BIT_SCK             7
#define PORT_SCK            PORTB
#define DDR_SCK             DDRB
#define PIN_SCK             PINB


/* ---------------- Port C ---------------- */

#define BIT_ADC8            0
#define PORT_ADC8           PORTC
#define DDR_ADC8            DDRC
#define PIN_ADC8            PINC

#define BIT_ADC9            1
#define PORT_ADC9           PORTC
#define DDR_ADC9            DDRC
#define PIN_ADC9            PINC

#define BIT_ADC10           2
#define PORT_ADC10          PORTC
#define DDR_ADC10           DDRC
#define PIN_ADC10           PINC

#define BIT_ADC11           3
#define PORT_ADC11          PORTC
#define DDR_ADC11           DDRC
#define PIN_ADC11           PINC

#define BIT_JTAG_TCK        4
#define PORT_JTAG_TCK       PORTC
#define DDR_JTAG_TCK        DDRC
#define PIN_JTAG_TCK        PINC

#define BIT_JTAG_TMS        5
#define PORT_JTAG_TMS       PORTC
#define DDR_JTAG_TMS        DDRC
#define PIN_JTAG_TMS        PINC

#define BIT_JTAG_TDO        6
#define PORT_JTAG_TDO       PORTC
#define DDR_JTAG_TDO        DDRC
#define PIN_JTAG_TDO        PINC

#define BIT_JTAG_TDI        7
#define PORT_JTAG_TDI       PORTC
#define DDR_JTAG_TDI        DDRC
#define PIN_JTAG_TDI        PINC


/* ---------------- Port D ---------------- */

#define BIT_RX              0
#define PORT_RX             PORTD
#define DDR_RX              DDRD
#define PIN_RX              PIND

#define BIT_TX              1
#define PORT_TX             PORTD
#define DDR_TX              DDRD
#define PIN_TX              PIND

#define BIT_RS232           2
#define PORT_RS232          PORTD
#define DDR_RS232           DDRD
#define PIN_RS232           PIND

#define BIT_DISPLAY_RESET   3
#define PORT_DISPLAY_RESET  PORTD
#define DDR_DISPLAY_RESET   DDRD
#define PIN_DISPLAY_RESET   PIND

#define BIT_IO_SS           4
#define PORT_IO_SS          PORTD
#define DDR_IO_SS           DDRD
#define PIN_IO_SS           PIND

#define BIT_ADC_CLK         5
#define PORT_ADC_CLK        PORTD
#define DDR_ADC_CLK         DDRD
#define PIN_ADC_CLK         PIND

#define BIT_WR              6
#define PORT_WR             PORTD
#define DDR_WR              DDRD
#define PIN_WR              PIND

#define BIT_RD              7
#define PORT_RD             PORTD
#define DDR_RD              DDRD
#define PIN_RD              PIND


/* ---------------- Port E ---------------- */

#define BIT_ALE             1
#define PORT_ALE            PORTE
#define DDR_ALE             DDRE
#define PIN_ALE             PINE

#endif // IO_DEFINES_H
