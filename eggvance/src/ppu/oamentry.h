#pragma once

#include "common/utility.h"
#include "common/macros.h"

enum GraphicsMode
{
    GFX_NORMAL     = 0b00,  // Normal
    GFX_ALPHA      = 0b01,  // Blend alpha
    GFX_WINDOW     = 0b10,  // Window
    GFX_PROHIBITED = 0b11   // Prohibited
};

struct OAMEntry
{
    template<unsigned index>
    inline void write(u16 half);

    int y;            // Y coordiante
    int affine;       // Affine flag
    int double_size;  // Double size flag (if affine)
    int disabled;     // Disabled flag (if not affine)
    int gfx_mode;     // Graphics mode (0 = normal, 1 = alpha blending, 2 = window, 3 = prohibited)
    int mosaic;       // Mosaic flag
    int color_mode;   // Color mode (1 = 256/1, 0 = 16/16)
    int shape;        // Object shape (0 = square, 1 = horizontal, 2 = vertical, 3 = prohibited)

    int x;         // X coordinate
    int paramter;  // Affine paramter (if affine)
    int flip_x;    // Horizontal flip (if not affine)
    int flip_y;    // Vertical flip (if not affine)
    int size;      // Object size

    int tile;          // Base tile number
    int priority;      // Priority
    int palette_bank;  // Palette bank for 16/16 color mode

    int width() const;
    int height() const;
};

template<unsigned index>
inline void OAMEntry::write(u16 half)
{
    static_assert(index == 0 || index == 2 || index == 4);

    switch (index)
    {
    case 0:
        y           = bits< 0, 8>(half);
        affine      = bits< 8, 1>(half);
        double_size = bits< 9, 1>(half);
        disabled    = bits< 9, 1>(half);
        gfx_mode    = bits<10, 2>(half);
        mosaic      = bits<12, 1>(half);
        color_mode  = bits<13, 1>(half);
        shape       = bits<14, 2>(half);
        break;

    case 2:
        x        = bits< 0, 9>(half);
        paramter = bits< 9, 5>(half);
        flip_x   = bits<12, 1>(half);
        flip_y   = bits<13, 1>(half);
        size     = bits<14, 2>(half);
        break;

    case 4:
        tile         = bits< 0, 10>(half);
        priority     = bits<10,  2>(half);
        palette_bank = bits<12,  4>(half);
        break;

    default:
        UNREACHABLE;
        break;
    }
}
