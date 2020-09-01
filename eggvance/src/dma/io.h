#pragma once

#include <functional>

#include "base/bit.h"
#include "base/register.h"

class DmaCount : public RegisterW<2>
{
public:
    uint count(uint id) const
    {
        if (value == 0)
            return id < 3 ? 0x4000 : 0x1'0000;
        else
            return value;
    }
};

class DmaControl : public Register<2>
{
public:
    static constexpr uint kEnable = 1 << 15;

    template<uint Index, uint Mask>
    void write(u8 byte)
    {
        Register<kSize, kMask>::write<Index, Mask>(byte);

        if (Index == 0)
        {
            dadcnt = bit::seq<5, 2>(byte);
            sadcnt = bit::seq<7, 1>(byte) << 0 | (sadcnt & ~0x1);
        }
        else
        {
            uint was_enabled = enable;

            sadcnt = bit::seq<0, 1>(byte) << 1 | (sadcnt & ~0x2);
            repeat = bit::seq<1, 1>(byte);
            word   = bit::seq<2, 1>(byte);
            drq    = bit::seq<3, 1>(byte);
            timing = bit::seq<4, 2>(byte);
            irq    = bit::seq<6, 1>(byte);
            enable = bit::seq<7, 1>(byte);

            reload = !was_enabled && enable;

            emit_immediate();
        }
    }

    uint dadcnt = 0;
    uint sadcnt = 0;
    uint repeat = 0;
    uint word   = 0;
    uint drq    = 0;
    uint timing = 0;
    uint irq    = 0;
    uint enable = 0;
    uint reload = 0;

    std::function<void(void)> emit_immediate;
};
