#pragma once

#include "../field.h"

class Dispstat
{
public:
    Dispstat(u16& data);

    Field<0, 1> vblank_flag;
    Field<1, 1> hblank_flag;
    Field<2, 1> vcount_flag;
    Field<3, 1> vblank_enable;
    Field<4, 1> hblank_enable;
    Field<5, 1> vcount_enable;
    Field<8, 8> vcount_trigger;
};
