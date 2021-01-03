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

        std::function<void(void)> clear_fifo;
    };

    DmaSoundControl()
    {
        if (config.bios_skip)
        {
            volume             = 2;
            channels[0].volume = 1;
            channels[1].volume = 1;
        }
    }

    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        if (Index == 0)
        {
            volume             = bit::seq<0, 2>(byte);
            channels[0].volume = bit::seq<2, 1>(byte);
            channels[1].volume = bit::seq<3, 1>(byte);
        }
        else
        {
            channels[0].enable_r = bit::seq<0, 1>(byte);
            channels[0].enable_l = bit::seq<1, 1>(byte);
            channels[0].timer    = bit::seq<2, 1>(byte);
            channels[1].enable_r = bit::seq<4, 1>(byte);
            channels[1].enable_l = bit::seq<5, 1>(byte);
            channels[1].timer    = bit::seq<6, 1>(byte);

            if (byte & (1 << 3)) channels[0].clear_fifo();
            if (byte & (1 << 7)) channels[1].clear_fifo();
        }
    }

    uint volume = 0;
    Channel channels[2];
};

class SoundBias : public Register<u16>
{
public:
    SoundBias()
    {
        bias = config.bios_skip << 8;
    }

    template<uint Index>
    void write(u8 byte)
    {
        Register::write(byte);

        bias = bit::seq<1, 9>(value);

        if (Index == 1)
        {
            sampling = 0;
        }
    }

    uint bias     = 0;
    uint sampling = 0;
};
