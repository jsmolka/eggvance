#pragma once

#include "common/field.h"

class Attribute0
{
public:
    Attribute0(u16 data);

    Field<u16,  0, 8> y;            // Y coordiante / top of sprite (0..255)
    Field<u16,  8, 1> rotation;     // Rotation / scaling flag (1 = on, 0 = off)
    Field<u16,  9, 1> double_size;  // If rotation: double size flag (1 = double, 0 = normal)
    Field<u16,  9, 1> hide;         // If not rotation: disabled flag (1 = not displayed)
    Field<u16, 10, 2> gfx_mode;     // Graphics mode (0 = normal, 1 = alpha blending, 2 = window, 3 = prohibited)
    Field<u16, 12, 1> mosaic;       // Mosaic enable (1 = enabled)
    Field<u16, 13, 1> color_mode;   // Color mode (1 = 256 colors, 0 = 16 colors)
    Field<u16, 14, 2> obj_shape;    // Sprite shape (0 = square, 1 = horizontal, 2 = vertical, 3 = prohibited)

private:
    u16 data;
};

