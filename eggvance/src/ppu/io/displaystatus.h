#pragma once

#include "common/bits.h"
#include "common/register.h"

class DisplayStatus : public RegisterRW<2>
{
public:
    template<uint index>
    inline u8 read() const
    {
        static_assert(index < 2);

        if (index == 0)
            return data[0] | (vblank << 0) | (hblank << 1) | (vmatch << 2);
        else
            return data[1];
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            vblank_irq = bits::seq<3, 1>(byte);
            hblank_irq = bits::seq<4, 1>(byte);
            vmatch_irq = bits::seq<5, 1>(byte);
        }
        else
        {
            vcompare = byte;
        }
    }

    uint vblank     = 0;
    uint hblank     = 0;
    uint vmatch     = 0;
    uint vblank_irq = 0;
    uint hblank_irq = 0;
    uint vmatch_irq = 0;
    uint vcompare   = 0;
};
