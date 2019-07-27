#pragma once

#include "../register.h"

struct DisplayStatus
{
    int vblank;          // V-Blank flag (set in lines 160..226, not 227)
    int hblank;          // H-Blank flag (toggled in all lines 0..227)
    int vcount_match;    // V-Count flag (set in selected line)
    int vblank_irq;      // V-Blank IRQ enable
    int hblank_irq;      // H-Blank IRQ enable
    int vcount_irq;      // V-Count IRQ enable
    int vcount_compare;  // Compare value for V-Count
};
