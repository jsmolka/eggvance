#pragma once

#include "../field.h"

class Bgcnt
{
public:
    Bgcnt(u16& data);

    u16& data;

    Field< 0, 2> priority;
    Field< 2, 2> tile_base;
    Field< 6, 1> mosaic;
    Field< 7, 1> palette;
    Field< 8, 4> map_base;
    Field<13, 1> wraparound;
    Field<14, 2> screen_size;

    u32 tileAddr() const;
    u32 mapAddr() const;
};