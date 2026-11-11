#include "pch.h"
#define BAUD 9600
#define UBRR ((F_CPU / BAUD / 16) - 1)
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include <stdio.h>
#include "helpers.h"
#include "gamepad.h"
#include "spi.h"
#include "oled.h"
#include "framebuffer.h"
#include "oled_text.h"
#include "screensaver.h"
#include "menu.h"
#include "io_defines.h"
#include "can.h"
#include "state.h"

#define EVER ;;

int main()
{
    uart_init(UBRR);
    uart_link_printf();
    printf("\n\nUART initialized\n");
    _delay_ms(1);
    SPI_MasterInit();
    gamepadInit();
    oledInit();
    oledSetFullWindow();
    oledWriteCmd(0xAF);
    
    can_init(CAN_MODE_NORMAL);
    // Enable external SRAM/XMEM
    MCUCR |= 1 << SRE;
    // Disable high-order bits in use by JTAG
    SFIOR = 1 << XMM2;

    //////////
    // Timer OC1A setup
    DDRD |= (1 << 5);
    TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << WGM11) | (0 << WGM10);
    TCCR1B = (1 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);

    ICR1 = 2;
    OCR1A = 1;
    // End Timer setup
    //////////CH1

    Gamepad gp;
    volatile char *adc_mmio = (char *) 0x1000; 
    CalibrationData cd;
    // calibrate(adc_mmio, &cd);

    screensaverInit();
    oledFbClear();
    oledFbFlush();
    int c = 0;

    MenuState state;
    MenuItem settingsItems[] = {
        {"PING PONG!!!", NULL, NULL },
        {"Screensaver", screensaverInit, NULL },
        {"Calibrate", NULL, NULL },
        {"Button Mapping", NULL, NULL },
        {"Joystick test", NULL, NULL }};
    Menu settingsMenu = { "Settings", settingsItems, sizeof(settingsItems) / sizeof(settingsItems[0]), NULL };
    menuInit(&state, &settingsMenu);
    menuDraw(&state);
    const int menuDelay = 200;

    int led = 0;
    DDR_CAN_SS |= 1;

    for (EVER)
    {
        readGamepadSPI(&gp);
        stateUpdate(&state, &gp);
    }
}
