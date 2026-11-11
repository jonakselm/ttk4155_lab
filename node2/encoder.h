#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include "sam.h"

/* Motor encoder on the shield, read by TC2 in quadrature decoder mode:
 *   D5 = PC25 = TIOA6 (channel A)
 *   D4 = PC26 = TIOB6 (channel B)
 * D4 is also wired to PA29 (missing from the Arduino pinout), so leave
 * PA29 as an input and never make it an output. */

void initEncoder(void);
int32_t readEncoder(void);      /* position in encoder counts, starts at 0 */

#endif // ENCODER_H
