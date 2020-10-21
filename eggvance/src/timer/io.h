#pragma once

#include <functional>

#include "base/register.h"

class TimerCount : public Register<u16>
{
public:
    template<uint Index>
    u8 read() const
    {
        run_channels();

        return Register::read<Index>();
    }

    template<uint Index>
    void write(u8 byte)
    {
        static_assert(Index < kSize);

        reinterpret_cast<u8*>(&initial)[Index] = byte;
    }

    u16 initial = 0;

    std::function<void(void)> run_channels;
};

class TimerControl : public Register<u16, 0x00C7>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        static constexpr uint kPrescalers[8] = { 1, 64, 256, 1024, 1, 1, 1, 1 };

        Register::write<Index>(byte);

        if (Index == 0)
        {
            run_channels();

            uint was_enabled = enable;

            prescaler = kPrescalers[bit::seq<0, 3>(byte)];
            cascade   = bit::seq<2, 1>(byte);
            irq       = bit::seq<6, 1>(byte);
            enable    = bit::seq<7, 1>(byte);

            update_channel(!was_enabled && enable);
        }
    }

    uint prescaler = 1;
    uint cascade   = 0;
    uint irq       = 0;
    uint enable    = 0;

    std::function<void(void)> run_channels;
    std::function<void(bool)> update_channel;
};
