#pragma once

#include "base/bits.h"

class Mosaic
{
public:
    struct Block
    {
        inline void write(u8 byte)
        {
            x = bits::seq<0, 4>(byte) + 1;
            y = bits::seq<4, 4>(byte) + 1;
        }

        inline uint mosaicX(uint value) const { return x * (value / x); }
        inline uint mosaicY(uint value) const { return y * (value / y); }

        uint x = 1;
        uint y = 1;
    };

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        if (index == 0)
            bgs.write(byte);
        else
            obj.write(byte);
    }

    Block bgs;
    Block obj;
};
