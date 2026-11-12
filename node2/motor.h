#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

/* Motor driver for the shield's A3959 in phase/enable mode:
 *   D20 = PB12 = PWM channel 0 (PWMH0): speed, duty 0..255
 *   D7  = PC23: direction                                   */

/* Set to 1 if the motor drives the carriage the wrong way */
#define MOTOR_DIR_INVERT 1

void motorInit(void);

/* speed: -255..255. The sign is the direction, the size is the PWM duty.
 * Positive should make the encoder count up (see MOTOR_DIR_INVERT).
 * Values outside the range are limited to +-255. */
void motorSet(int16_t speed);

#endif // MOTOR_H
