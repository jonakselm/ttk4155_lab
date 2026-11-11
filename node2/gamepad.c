#include "gamepad.h"
#include <stdio.h>

Gamepad canDecodeGamepadData(CanMsg *msg)
{
    Gamepad gp;
    gp.dword[0] = msg->dword[0];
    gp.dword[1] = msg->dword[1];

    return gp;
}
