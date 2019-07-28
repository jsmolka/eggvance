#pragma once

#include "common/integer.h"

struct BackgroundControl
{
    int priority;      // BG priority (0-3, 0 = highest)
    int tile_base;     // BG tile base block (0-3, in units of 16kb)
    int mosaic;        // Mosaic (1 = enable)
    int palette_type;  // Palettes type (0 = 16/16, 1 = 256/1)
    int map_base;      // BG map base block (0-31, in units of 2kb)
    int wraparound;    // Display area overflow (1 = enable)
    int screen_size;   // Screen size

    u32 tileBase() const;
    u32 mapBase() const;

    int width() const;
    int height() const;
    int affineSize() const;

    static const int sizes[4][2];
    static const int affine_sizes[4];
};
