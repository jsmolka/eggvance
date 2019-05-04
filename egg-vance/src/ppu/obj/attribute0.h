#pragma once

#include "common/bitvalue.h"

class Attribute0
{
public:
    Attribute0(int data);

    BitValue< 0, 8> y;            // Y coordiante / top of sprite (0..255)
    BitValue< 8, 1> rotation;     // Rotation / scaling flag (1 = on, 0 = off)
    BitValue< 9, 1> double_size;  // If rotation: double size flag (1 = double, 0 = normal)
    BitValue< 9, 1> hide;         // If not rotation: disabled flag (1 = not displayed)
    BitValue<10, 2> gfx_mode;     // Graphics mode (0 = normal, 1 = alpha blending, 2 = window, 3 = prohibited)
    BitValue<12, 1> mosaic;       // Mosaic enable (1 = enabled)
    BitValue<13, 1> color_mode;   // Color mode (1 = 256 colors, 0 = 16 colors)
    BitValue<14, 2> obj_shape;    // Sprite shape (0 = square, 1 = horizontal, 2 = vertical, 3 = prohibited)
};

