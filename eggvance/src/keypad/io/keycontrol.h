#pragma once

#include "common/bits.h"
#include "common/register.h"

class KeyControl : public RegisterRW<2>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        u8* mask = reinterpret_cast<u8*>(&this->mask);

        if (index == 0)
        {
            mask[0] = byte;
        }
        else
        {
            mask[1] = bits::seq<0, 2>(byte);
            irq     = bits::seq<6, 1>(byte);
            logic   = bits::seq<7, 1>(byte);
        }
    }

    uint mask  = 0;
    uint irq   = 0;
    uint logic = 0;
};
