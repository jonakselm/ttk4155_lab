#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "common.h"
#include "can.h"

Gamepad canDecodeGamepadData(CanMsg *msg);

#endif // GAMEPAD_H
