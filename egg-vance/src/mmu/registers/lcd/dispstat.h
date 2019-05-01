#pragma once

#include "../register.h"

class Dispstat : public Register
{
public:
    Dispstat(u16& data);

    using Register::operator=;
    using Register::operator unsigned int;

    Field<0, 1> vblank;          // V-Blank flag, set in lines 160..226, not 227 (1 = V-Blank)
    Field<1, 1> hblank;          // H-Blank flag, toggled in all lines 0..227 (1 = H-Blank)
    Field<2, 1> vcount_match;    // V-Count flag, set in selected line (1 = match)
    Field<3, 1> vblank_irq;      // V-Blank IRQ enable (1 = enable)
    Field<4, 1> hblank_irq;      // H-Blank IRQ enable (1 = enable)
    Field<5, 1> vcount_irq;      // V-Count IRQ enable (1 = enable)
    Field<8, 8> vcount_compare;  // Compare value for V-Count, set match if equal and request interrupt if enabled
};
