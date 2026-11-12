#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

/* Servo on the shield: D21 = PB13 = PWM channel 1 (PWMH1).
 * 1 MHz PWM clock, so one count = 1 us: 20 ms period, pulse 0.9-2.1 ms. */
#define SERVO_MIN_US     900
#define SERVO_CENTER_US 1500
#define SERVO_MAX_US    2100

void servoInit(void);                   /* no pulse (servo off) until the first servoSet*() call */
void servoSetUs(uint32_t us);           /* pulse width, always kept in 900..2100 */
void servoSetJoystick(uint8_t value);   /* 0..255 -> 900..2100 us */
void servoOff(void);                    /* no pulse: the servo lets go (same as at startup) */

#endif // SERVO_H
