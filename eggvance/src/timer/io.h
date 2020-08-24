#pragma once

#include "base/register.h"

struct TimerIo
{
    class Count : public Register<2>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            static_assert(Index < kSize);

            reinterpret_cast<u8*>(&initial)[Index] = byte;
        }

        u16 initial{};
    } count;

    class Control : public Register<2, 0x00C7>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            static constexpr uint kPrescalers[4] = { 1, 64, 256, 1024 };

            Register<kSize, kMask>::write<Index>(byte);

            if (Index == 0)
            {
                cascade = bit::seq<2, 1>(byte);
                irq     = bit::seq<6, 1>(byte);
                enable  = bit::seq<7, 1>(byte);

                if (cascade)
                    prescaler = 1;
                else
                    prescaler = kPrescalers[bit::seq<0, 2>(byte)];
            }
        }

        uint prescaler{1};
        uint cascade{};
        uint irq{};
        uint enable{};
    } control;
};
