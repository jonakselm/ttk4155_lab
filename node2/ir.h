#ifndef IR_H
#define IR_H

#include <stdint.h>

/* IR receiver on Arduino A8 = PB17 = ADC channel 10 (AD10).
 * Note: the Arduino pinout calls A8 "ADC[8]", but on the chip it is AD10.
 * The signal must stay within 0..3.3 V. */

#define IR_ADC_CHANNEL  10

void     irInit(void);
uint16_t irRead(void);          /* raw ADC value, 0..4095 */

#endif // IR_H
