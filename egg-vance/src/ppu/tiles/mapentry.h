#pragma once

#include "common/bitvalue.h"

class MapEntry
{
public:
    MapEntry(int data);

    BitValue< 0, 10> tile;     // Tile index in tiles
    BitValue<10,  1> flip_x;   // Flip tile horizontally 
    BitValue<11,  1> flip_y;   // Flip tile vertically
    BitValue<12,  4> palette;  // Palette bank (used for 16-bit color mode, otherwise ignored)
};
