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
        run_channels();

        return Register<kSize>::read<Index>();
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

class TimerControl : public Register<2, 0x00C7>
{
public:
    template<uint Index>
    void write(u8 byte)
    {
        static constexpr uint kPrescalers[8] = { 1, 64, 256, 1024, 1, 1, 1, 1 };

        if (Index > 0)
            return;

        Register<kSize, kMask>::write<Index>(byte);

        run_channels();

        uint was_enabled = enabled;

        prescaler = kPrescalers[bit::seq<0, 3>(byte)];
        cascade   = bit::seq<2, 1>(byte);
        irq       = bit::seq<6, 1>(byte);
        enabled   = bit::seq<7, 1>(byte);

        update_channel(!was_enabled && enabled);
    }

    uint prescaler = 1;
    uint cascade   = 0;
    uint irq       = 0;
    uint enabled   = 0;

    std::function<void(void)> run_channels;
    std::function<void(bool)> update_channel;
};
