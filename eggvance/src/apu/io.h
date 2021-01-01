#pragma once

#include <functional>

#include "base/config.h"
#include "base/register.h"

class DmaSoundControl : public Register<u16, 0x770F>
{
public:
    struct Channel
    {
        uint volume   = 0;
        uint enable_r = 0;
        uint enable_l = 0;
        uint timer    = 0;
    };

    DmaSoundControl()
    {
        if (config.bios_skip)
        {
            volume   = 2;
            a.volume = 1;
            b.volume = 1;
        }
    }

    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        if (Index == 0)
        {
            volume   = bit::seq<0, 2>(byte);
            a.volume = bit::seq<2, 1>(byte);
            b.volume = bit::seq<3, 1>(byte);
        }
        else
        {
            a.enable_r = bit::seq<0, 1>(byte);
            a.enable_l = bit::seq<1, 1>(byte);
            a.timer    = bit::seq<2, 1>(byte);
            b.enable_r = bit::seq<4, 1>(byte);
            b.enable_l = bit::seq<5, 1>(byte);
            b.timer    = bit::seq<6, 1>(byte);

            if (byte & (1 << 3)) reset_fifo_a();
            if (byte & (1 << 7)) reset_fifo_b();
        }
    }

    uint volume = 0;
    Channel a;
    Channel b;

    std::function<void(void)> reset_fifo_a;
    std::function<void(void)> reset_fifo_b;
};
