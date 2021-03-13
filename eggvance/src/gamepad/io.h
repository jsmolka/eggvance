#pragma once

#include "base/register.h"

class KeyInput : public RegisterR<u16, 0x03FF>
{
public:
    KeyInput()
    {
        value = kMask;
    }
};

class KeyControl : public Register<u16, 0xC3FF>
{
public:
    template<uint Index>
    void write(u8 byte);

    bool raisesIrq(const KeyInput& input)
    {
        enum Condition
        {
            kConditionAny,
            kConditionAll
        };

        uint value = ~input.value & KeyInput::kMask;

        return cond == kConditionAll
            ? (value == mask)
            : (value &  mask);
    }

    uint mask = 0;
    uint irq  = 0;
    uint cond = 0;
};
