#pragma once

#include "common/bits.h"
#include "common/register.h"

class DMAControl : public RegisterRW<2>
{
public:
    template<uint index>
    inline u8 read() const
    {
        static_assert(index < 2);

        if (index == 0)
            return data[0];
        else
            return (data[1] & ~(1 << 7)) | (enable << 7);
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            dadcnt = bits<5, 2>(byte);
            sadcnt = bits<7, 1>(byte) << 0 | (sadcnt & ~0x1);
        }
        else
        {
            sadcnt = bits<0, 1>(byte) << 1 | (sadcnt & ~0x2);
            repeat = bits<1, 1>(byte);
            word   = bits<2, 1>(byte);
            timing = bits<4, 2>(byte);
            irq    = bits<6, 1>(byte);
            reload = bits<7, 1>(byte) ^ enable;
            enable = bits<7, 1>(byte);
        }
    }

    uint sadcnt = 0;
    uint dadcnt = 0;
    uint repeat = 0;
    uint word   = 0;
    uint timing = 0;
    uint irq    = 0;
    uint enable = 0;
    bool reload = false;
};
