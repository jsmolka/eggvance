#pragma once

#include "common/integer.h"

struct KeyControl
{
    union
    {
        u8 bytes[2];
        u16 mask;
    };
    int irq;     // IRQ enable
    int logic;   // IRQ logic (0 = or, 1 = and)
};
