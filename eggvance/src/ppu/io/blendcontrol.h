#pragma once

#include "base/bits.h"
#include "base/register.h"

class BlendControl : public RegisterRW<2>
{
public:
    enum Mode
    {
        DISABLED = 0b00,
        ALPHA    = 0b01,
        WHITE    = 0b10,
        BLACK    = 0b11
    };

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            upper = bits::seq<0, 6>(byte);
            mode  = bits::seq<6, 2>(byte);
        }
        else
        {
            lower = bits::seq<0, 6>(byte);
        }
    }

    uint mode  = 0;
    uint upper = 0;
    uint lower = 0;
};
