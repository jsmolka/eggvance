#pragma once

#include "common/integer.h"

struct KeyControl
{
    union
    {
        u8  keys_b[2];  // Key mask bytes
        u16 keys;       // Key mask
    };
    int irq;     // IRQ enable
    int logic;   // IRQ logic (0 = or, 1 = and)
};
