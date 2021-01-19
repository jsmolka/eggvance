#pragma once

#include <functional>

#include "base/config.h"
#include "base/register.h"

class DirectSoundControl : public Register<u16, 0x770F>
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

    DirectSoundControl()
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

class PsgSoundControl : public Register<u16, 0xFF77>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        if (Index == 0)
        {
            volume_r = bit::seq<0, 3>(byte);
            volume_l = bit::seq<4, 3>(byte);
        }
        if (Index == 1)
        {
            enable_r[0] = bit::seq<0, 1>(byte);
            enable_r[1] = bit::seq<1, 1>(byte);
            enable_r[2] = bit::seq<2, 1>(byte);
            enable_r[3] = bit::seq<3, 1>(byte);
            enable_l[0] = bit::seq<4, 1>(byte);
            enable_l[1] = bit::seq<5, 1>(byte);
            enable_l[2] = bit::seq<6, 1>(byte);
            enable_l[3] = bit::seq<7, 1>(byte);
        }
    }

    uint volume_r = 0;
    uint volume_l = 0;
    uint enable_r[4] = {};
    uint enable_l[4] = {};
};

class SoundControl : public Register<u16, 0x0080>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        if (Index == 0)
        {
            enable = bit::seq<7, 1>(byte);
        }
    }

    uint enable = 0;
};

class SoundBias : public Register<u16, 0xC3FF>
{
public:
    SoundBias()
    {
        if (config.bios_skip)
        {
            write<0>(0x00);
            write<1>(0x02);
        }
    }

    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        level = bit::seq<0, 10>(value);

        if (Index == 1)
        {
            aplitude = bit::seq<6, 2>(byte);
        }
    }

    uint level    = 0;
    uint aplitude = 0;
};
