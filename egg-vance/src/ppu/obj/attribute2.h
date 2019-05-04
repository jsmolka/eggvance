#pragma once

#include "common/bitvalue.h"

class Attribute2
{
public:
    Attribute2(int data);

    BitValue< 0, 10> tile_number;     // Tile number (0..1023)
    BitValue<10,  2> priority;        // Priority (0 = highest) 
    BitValue<12,  4> palette_number;  // Palette number (0..15)
};
