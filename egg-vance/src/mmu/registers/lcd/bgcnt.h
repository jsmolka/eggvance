#pragma once

#include "../register.h"

class Bgcnt : public Register
{
public:
    Bgcnt(u16& data);

    Field< 0, 2> priority;     // BG priority (0-3, 0 = highest)
    Field< 2, 2> tile_base;    // BG tile base block (0-3, in units of 16kb)
    Field< 6, 1> mosaic;       // Mosaic (1 = enable)
    Field< 7, 1> palette;      // Palettes (0 = 16/16, 1 = 256/1)
    Field< 8, 4> map_base;     // BG map base block (0-31, in units of 2kb)
    Field<13, 1> wraparound;   // Display area overflow (1 = enable)
    Field<14, 2> screen_size;  // Screen size

    u32 tileAddr() const;
    u32 mapAddr() const;
};