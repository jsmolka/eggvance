#pragma once

#include "register.h"
#include "common/bits.h"

class KeyControl : public Register<KeyControl, 2>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        u8* maskb = reinterpret_cast<u8*>(&mask);

        if (index == 0)
        {
            maskb[0] = byte;
        }
        else
        {
            maskb[1] = bits<0, 2>(byte);
            irq      = bits<6, 1>(byte);
            logic    = bits<7, 1>(byte);
        }
    }

    uint mask  = 0;
    uint irq   = 0;
    uint logic = 0;
};
