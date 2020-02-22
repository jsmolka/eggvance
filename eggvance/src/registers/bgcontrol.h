#pragma once

#include "register.h"
#include "common/bits.h"
#include "common/macros.h"
#include "ppu/dimensions.h"

class BGControl : public TRegister<BGControl, 2>
{
public:
    BGControl()
    {
        write<0>(0);
        write<1>(0);
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            priority   = bits<0, 2>(byte);
            tile_block = bits<2, 2>(byte);
            mosaic     = bits<6, 1>(byte);
            color_mode = bits<7, 1>(byte);

            base_tile = 0x4000 * tile_block;
        }
        else
        {
            map_block   = bits<0, 5>(byte);
            wraparound  = bits<5, 1>(byte);
            screen_size = bits<6, 2>(byte);

            dims_reg.w = 256 << bits<0, 1>(screen_size);
            dims_reg.h = 256 << bits<1, 1>(screen_size);
            dims_aff.w = 128 << screen_size;
            dims_aff.h = 128 << screen_size;

            base_map  = 0x0800 * map_block;
        }
    }

    uint priority;
    uint tile_block;
    uint mosaic;
    uint color_mode;
    uint map_block;
    uint wraparound;
    uint screen_size;

    Dimensions dims_aff;
    Dimensions dims_reg;

    u32 base_map;
    u32 base_tile;
};
