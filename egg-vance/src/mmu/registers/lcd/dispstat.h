#pragma once

#include "../field.h"

class Dispstat
{
public:
    Dispstat(u16& data);

    u16& data;

    Field<0, 1> vblank;
    Field<1, 1> hblank;
    Field<2, 1> vcount_match;
    Field<3, 1> vblank_irq;
    Field<4, 1> hblank_irq;
    Field<5, 1> vcount_irq;
    Field<8, 8> vcount_trigger;
};
