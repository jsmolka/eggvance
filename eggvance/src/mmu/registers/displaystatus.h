#pragma once

#include "common/integer.h"

struct DisplayStatus
{
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int vblank;          // V-Blank flag (set in lines 160..226, not 227)
    int hblank;          // H-Blank flag (toggled in all lines 0..227)
    int vmatch;          // V-Count match flag (set in selected line)
    int vblank_irq;      // V-Blank IRQ enable
    int hblank_irq;      // H-Blank IRQ enable
    int vmatch_irq;      // V-Count match IRQ enable
    int vcount_compare;  // Compare value for V-Count
};
