#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct BackgroundControl
{
    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    int priority;      // BG priority (0-3, 0 = highest)
    int tile_block;    // BG tile base block (0-3, in units of 16kb)
    int mosaic;        // Mosaic (1 = enable)
    int palette_type;  // Palette type (0 = 16/16, 1 = 256/1)
    int map_block;     // BG map base block (0-31, in units of 2kb)
    int wraparound;    // Display area overflow (1 = enable)
    int screen_size;   // Screen size

    int size() const;
    int width() const;
    int height() const;
};

template<unsigned index>
inline void BackgroundControl::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0:
        priority     = bits<0, 2>(byte);
        tile_block   = bits<2, 2>(byte);
        mosaic       = bits<6, 1>(byte);
        palette_type = bits<7, 1>(byte);
        break;

    case 1:
        map_block   = bits<0, 5>(byte);
        wraparound  = bits<5, 1>(byte);
        screen_size = bits<6, 2>(byte);
        break;

    default:
        UNREACHABLE;
        break;
    }
}
