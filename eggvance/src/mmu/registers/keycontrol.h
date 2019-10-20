#pragma once

#include "common/integer.h"

class KeyControl
{
public:
    void reset();

    void write(int index, u8 byte);

    int keys;       // Key mask
    int irq;        // IRQ enable
    int irq_logic;  // IRQ logic (0 = or, 1 = and)
};
