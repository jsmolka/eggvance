#pragma once

#include "base/bits.h"
#include "base/register.h"
#include "ppu/dimensions.h"

class BGControl : public RegisterRW<2>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            priority   = bits::seq<0, 2>(byte);
            tile_block = bits::seq<2, 2>(byte) * 0x4000;
            mosaic     = bits::seq<6, 1>(byte);
            color_mode = bits::seq<7, 1>(byte);
        }
        else
        {
            map_block  = bits::seq<0, 5>(byte) * 0x0800;
            wraparound = bits::seq<5, 1>(byte);
            dimensions = bits::seq<6, 2>(byte);
        }
    }

    inline Dimensions dimsReg() const
    {
        return Dimensions(
            256 << bits::seq<0, 1>(dimensions),
            256 << bits::seq<1, 1>(dimensions)
        );
    }

    inline Dimensions dimsAff() const
    {
        return Dimensions(
            128 << dimensions,
            128 << dimensions
        );
    }

    uint priority   = 0;
    uint tile_block = 0;
    uint mosaic     = 0;
    uint color_mode = 0;
    uint map_block  = 0;
    uint wraparound = 0;
    uint dimensions = 0;
};
