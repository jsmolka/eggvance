#pragma once

#include "common/field.h"

class Attribute2
{
public:
    Attribute2(u16 data);

    Field<u16,  0, 10> tile_number;     // Tile number (0..1023)
    Field<u16, 10,  2> priority;        // Priority (0 = highest) 
    Field<u16, 12,  4> palette_number;  // Palette number (0..15)

private:
    u16 data;
};
