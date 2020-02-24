#pragma once

#include "register.h"
#include "common/bits.h"

class Mosaic : public TRegister<Mosaic, 2>
{
public:
    struct Block
    {
        inline uint mosaicX(uint value) const { return x * (value / x); }
        inline uint mosaicY(uint value) const { return y * (value / y); }

        uint x = 1;
        uint y = 1;
    };

    template<uint index>
    inline u8 read() const = delete;

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        if (index == 0)
        {
            bgs.x = bits<0, 4>(byte) + 1;
            bgs.y = bits<4, 4>(byte) + 1;
        }
        else
        {
            obj.x = bits<0, 4>(byte) + 1;
            obj.y = bits<4, 4>(byte) + 1;
        }
    }

    Block bgs;
    Block obj;
};
