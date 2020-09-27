#pragma once

#include "base/register.h"

class KeyInput : public RegisterR<u16, 0x03FF>
{
public:
    KeyInput()
        : RegisterR(kMask) {}
};

class KeyControl : public Register<u16, 0xC3FF>
{
public:
    enum Condition
    {
        kConditionAny,
        kConditionAll
    };

    template<uint Index>
    void write(u8 byte)
    {
        Register::write<Index>(byte);

        mask = bit::seq<0, 10>(value);

        if (Index == 1)
        {
            irq  = bit::seq<6, 1>(byte);
            cond = bit::seq<7, 1>(byte);
        }
    }

    bool raisesIrq(const KeyInput& input)
    {
        u16 value = ~input.value & KeyInput::kMask;

        return cond == kConditionAll
            ? (value == mask)
            : (value &  mask);
    }

    uint mask = 0;
    uint irq  = 0;
    uint cond = 0;
};
