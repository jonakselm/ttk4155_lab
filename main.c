#include "pch.h"
#define BAUD 9600
#define UBRR ((F_CPU / BAUD / 16) - 1)
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include <stdio.h>
#include "sram_test.h"
#include "helpers.h"
#include "gamepad.h"
#include "spi.h"
#include "oled.h"
#include "framebuffer.h"
#include "oled_text.h"
#include "screensaver.h"
#include "menu.h"
#include "io_defines.h"

#define EVER ;;


int main()
{
    //////////////////////////
    // Add an array of callbacks
    //  where a state variable can
    //  decide the index of the callback
    //  to use in the loop
    uart_init(UBRR);
    uart_link_printf();
    printf("\n\nUART initialized\n");
    _delay_ms(1);
    SPI_MasterInit();
    gamepadInit();
    oledInit();
    oledSetFullWindow();
    oledWriteCmd(0xAF);
    // oledReset();
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

    // screensaverInit();
    oledFbClear();
    oledFbFlush();
    int c = 0;

    MenuState state;
    MenuItem settingsItems[] = {
        {"Screensaver", screensaverInit, NULL },
        {"Noge", NULL, NULL },
        {"Aent", NULL, NULL },
        {"Tull", NULL, NULL },
        {"Jada", NULL, NULL },
        {"Neivel", NULL, NULL },
        {"Calibrate", NULL, NULL },
        {"Button Mapping", NULL, NULL }};
    Menu settingsMenu = { "Settings", settingsItems, sizeof(settingsItems) / sizeof(settingsItems[0]), NULL };
    menuInit(&state, &settingsMenu);
    menuDraw(&state);
    const int menuDelay = 200;
    uint8_t buf[2];

    int led = 0;

    for (EVER)
    {
        // screensaverStep();
        // led = !led;
        // readGamepadSPI(&gp);

        oledFbClear();
        oledTextSetColumn(0);
        oledTextSetLine(0);
        oled_printf("DISP_SS: %i\nIO_SS: %i\nNoge %i", (PIN_DISPLAY_SS & (1 << BIT_DISPLAY_SS)) >> BIT_DISPLAY_SS, (PIN_IO_SS & (1 << BIT_IO_SS)) >> BIT_IO_SS, c++);
        oledFbFlush();
        
        // if (gp.joyY > 200)
        // {
        //     menuUp(&state);
        //     // _delay_ms(menuDelay);
        // }
        // else if (gp.joyY < 50)
        // {
        //     menuDown(&state);
        //     // _delay_ms(menuDelay);
        // }
        // menuDraw(&state);
        
        // readCalibrated(adc_mmio, &cd, &gp);
        // oledFbClear();
        // oledTextCursor(0, 0);
        // oledFbClear();
        // oledTextSetColumn(0);
        // oledTextSetLine(0);
        // oled_printf("Joystick: (%3i, %3i)\n", gp.joyX, gp.joyY);
        // oledFbFlush();
        // oled_printf("Touch: (%3i, %3i)\n", gp.touchX, gp.touchY);
        // oledFbFlush();
        // printf("Noge lange tekst:    %3i, %3i, %3i, %3i\r",
        //         gp.joyX, gp.joyY, gp.touchX, gp.touchY);
        // printf("ADC:    %3i%%, %3i%%, %3i%%, %3i%%\r", valueToPercentage(gp.joyX),
        //         valueToPercentage(gp.joyY), valueToPercentage(gp.touchX),
        //         valueToPercentage(gp.touchY));
    }
}
