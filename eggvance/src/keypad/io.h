#pragma once

#include "base/bits.h"
#include "base/register.h"

struct KeypadIo
{
    class Input : public RegisterR<2, 0x03FF>
    {
    public:
        Input() { value = kMask; }
    } input;

    class Control : public Register<2, 0xC3FF>
    {
    public:
        template<uint Index>
        void write(u8 byte)
        {
            Register<kSize, kMask>::write<Index>(byte);

            if (Index == 1)
            {
                irq   = bits::seq<6, 1>(byte);
                logic = bits::seq<7, 1>(byte);
            }
            mask = bits::seq<0, 10>(value);
        }

        uint mask{};
        uint irq{};
        uint logic{};
    } control;
};
