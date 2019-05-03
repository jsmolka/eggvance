#pragma once

#include "../register.h"

class Bgcnt : public Register<u16>
{
public:
    Bgcnt(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    Field<u16,  0, 2> priority;     // BG priority (0-3, 0 = highest)
    Field<u16,  2, 2> tile_base;    // BG tile base block (0-3, in units of 16kb)
    Field<u16,  6, 1> mosaic;       // Mosaic (1 = enable)
    Field<u16,  7, 1> palette;      // Palettes (0 = 16/16, 1 = 256/1)
    Field<u16,  8, 4> map_base;     // BG map base block (0-31, in units of 2kb)
    Field<u16, 13, 1> wraparound;   // Display area overflow (1 = enable)
    Field<u16, 14, 2> screen_size;  // Screen size

    u32 tileAddr() const;
    u32 mapAddr() const;
};