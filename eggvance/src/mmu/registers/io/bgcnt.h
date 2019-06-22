#pragma once

#include "../register.h"

class Bgcnt : public Register<u16>
{
public:
    Bgcnt(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16,  0, 2> priority;      // BG priority (0-3, 0 = highest)
    BitField<u16,  2, 2> tile_base;     // BG tile base block (0-3, in units of 16kb)
    BitField<u16,  6, 1> mosaic;        // Mosaic (1 = enable)
    BitField<u16,  7, 1> palette_type;  // Palettes type (0 = 16/16, 1 = 256/1)
    BitField<u16,  8, 5> map_base;      // BG map base block (0-31, in units of 2kb)
    BitField<u16, 13, 1> wraparound;    // Display area overflow (1 = enable)
    BitField<u16, 14, 2> screen_size;   // Screen size

    u32 tileBase() const;
    u32 mapBase() const;

    int width() const;
    int height() const;
    int affineSize() const;

private:
    static const int sizes[4][2];
    static const int affine_sizes[4];
};
