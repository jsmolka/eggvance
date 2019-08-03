#pragma once

#include "common/integer.h"

struct OAMEntry
{
    int y;            // Y coordiante
    int affine;       // Affine flag
    int double_size;  // Double size flag (if affine)
    int disabled;     // Disabled flag (if not affine)
    int gfx_mode;     // Graphics mode (0 = normal, 1 = alpha blending, 2 = window, 3 = prohibited)
    int mosaic;       // Mosaic flag
    int color_mode;   // Color mode (1 = 256/1, 0 = 16/16)
    int shape;        // Object shape (0 = square, 1 = horizontal, 2 = vertical, 3 = prohibited)

    union
    {
        u8  x_b[2];  // X coordinate bytes
        u16 x;       // X coordinate
    };
    int paramter;  // Affine paramter (if affine)
    int flip_h;    // Horizontal flip (if not affine)
    int flip_v;    // Vertical flip (if not affine)
    int size;      // Object size

    union
    {
        u8  tile_b[2];  // Base tile number bytes
        u16 tile;       // Base tile number
    };
    int priority;  // Priority
    int palette;   // Palette bank for 16/16 color mode

    int width() const;
    int height() const;
};
