#pragma once

#include "register.h"
#include "common/bits.h"
#include "ppu/dimensions.h"

class BGControl : public Register<BGControl, 2>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            priority   = bits<0, 2>(byte);
            tile_block = bits<2, 2>(byte) * 0x4000;
            mosaic     = bits<6, 1>(byte);
            color_mode = bits<7, 1>(byte);
        }
        else
        {
            map_block   = bits<0, 5>(byte) * 0x800;
            wraparound  = bits<5, 1>(byte);
            screen_size = bits<6, 2>(byte);
        }
    }

    inline Dimensions dimsReg() const
    {
        return Dimensions(
            256 << bits<0, 1>(screen_size),
            256 << bits<1, 1>(screen_size)
        );
    }

    inline Dimensions dimsAff() const
    {
        return Dimensions(
            128 << screen_size,
            128 << screen_size
        );
    }

    uint priority    = 0;
    uint tile_block  = 0;
    uint mosaic      = 0;
    uint color_mode  = 0;
    uint map_block   = 0;
    uint wraparound  = 0;
    uint screen_size = 0;
};
