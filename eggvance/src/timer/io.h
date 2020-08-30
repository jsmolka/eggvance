#pragma once

#include <functional>

#include "base/bit.h"
#include "base/register.h"

class TimerCount : public Register<2>
{
public:
    template<uint Index>
    u8 read() const
    {
        update_count();

        return Register<kSize>::read<Index>();
    }

    template<uint Index>
    void write(u8 byte)
    {
        static_assert(Index < kSize);

        reinterpret_cast<u8*>(&initial)[Index] = byte;
    }

    u16 initial = 0;

    std::function<void(void)> update_count;
};

class TimerControl : public Register<2, 0x00C7>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        static constexpr uint kPrescalers[4] = { 1, 64, 256, 1024 };

        if (Index > 0)
            return;

        Register<kSize, kMask>::write<Index>(byte);

        uint was_enabled = enable;

        update_count();

        cascade = bit::seq<2, 1>(byte);
        irq     = bit::seq<6, 1>(byte);
        enable  = bit::seq<7, 1>(byte);

        if (cascade)
            prescaler = 1;
        else
            prescaler = kPrescalers[bit::seq<0, 2>(byte)];

        update_timer(!was_enabled && enable);
    }

    uint prescaler = 1;
    uint cascade   = 0;
    uint irq       = 0;
    uint enable    = 0;

    std::function<void(void)> update_count;
    std::function<void(bool)> update_timer;
};
