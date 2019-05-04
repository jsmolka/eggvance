#pragma once

#include "common/field.h"

class Attribute1
{
public:
    Attribute1(u16 data);

    Field<u16,  0, 9> x;         // X coordinate (0..511)
    Field<u16,  9, 5> paramter;  // If rotation: Rotation / scaling paramter
    Field<u16, 12, 1> hor_flip;  // If not rotation: horizontal flip (1 = mirrored)
    Field<u16, 13, 1> ver_flip;  // If not rotation: vertical flip (1 = mirrored)
    Field<u16, 14, 2> obj_size;  // Object size (depends on shape)

private:
    u16 data;
};
