#pragma once

#include "../register.h"

class Dispstat : public Register<u16>
{
public:
    Dispstat(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    Field<u16, 0, 1> vblank;          // V-Blank flag, set in lines 160..226, not 227 (1 = V-Blank)
    Field<u16, 1, 1> hblank;          // H-Blank flag, toggled in all lines 0..227 (1 = H-Blank)
    Field<u16, 2, 1> vcount_match;    // V-Count flag, set in selected line (1 = match)
    Field<u16, 3, 1> vblank_irq;      // V-Blank IRQ enable (1 = enable)
    Field<u16, 4, 1> hblank_irq;      // H-Blank IRQ enable (1 = enable)
    Field<u16, 5, 1> vcount_irq;      // V-Count IRQ enable (1 = enable)
    Field<u16, 8, 8> vcount_compare;  // Compare value for V-Count, set match if equal and request interrupt if enabled
};
