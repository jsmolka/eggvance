#pragma once

#include "common/bitvalue.h"

class Attribute1
{
public:
    Attribute1(int data);

    BitValue< 0, 9> x;         // X coordinate (0..511)
    BitValue< 9, 5> paramter;  // If rotation: Rotation / scaling paramter
    BitValue<12, 1> flip_y;    // If not rotation: horizontal flip (1 = mirrored)
    BitValue<13, 1> flip_x;    // If not rotation: vertical flip (1 = mirrored)
    BitValue<14, 2> size;      // Object size (depends on shape)
};
