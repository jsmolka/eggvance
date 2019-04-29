#pragma once

#include "../field.h"

class Dispcnt
{
public:
    Dispcnt(u16& data);

    u16& data;

    Field< 0, 3> bg_mode;
    Field< 3, 1> gbc_mode;
    Field< 4, 1> frame_select;
    Field< 5, 1> process_hblank;
    Field< 6, 1> obj_mapping;
    Field< 7, 1> force_blank;
    Field< 8, 1> bg0;
    Field< 9, 1> bg1;
    Field<10, 1> bg2;
    Field<11, 1> bg3;
    Field<12, 1> obj;
    Field<13, 1> win0;
    Field<14, 1> win1;
    Field<15, 1> objwin;

    u32 frameAddr() const;
};
