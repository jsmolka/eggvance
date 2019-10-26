#include "arm.h"

#include <algorithm>

#include "common/macros.h"
#include "common/utility.h"

u32 ARM::lsl(u32 value, int amount, bool& carry) const
{
    if (amount != 0)
    {
        if (amount < 32)
        {
            carry = (value << (amount - 1)) >> 31;
            value <<= amount;
        }
        else
        {
            if (amount == 32)
                carry = value & 0x1;
            else
                carry = 0;

            value = 0;
        }
    }
    return value;
}

u32 ARM::lsr(u32 value, int amount, bool& carry, bool immediate) const
{
    if (amount != 0)
    {
        if (amount < 32)
        {
            carry = (value >> (amount - 1)) & 0x1;
            value >>= amount;
        }
        else
        {
            carry = 0;
            value = 0;
        }
    }
    else if (immediate)
    {
        carry = value >> 31;
        value = 0;
    }
    return value;
}

u32 ARM::asr(u32 value, int amount, bool& carry, bool immediate) const
{
    if (amount != 0)
    {
        if (amount < 32)
        {
            carry = (value >> (amount - 1)) & 0x1;
            value = static_cast<s32>(value) >> amount;
        }
        else
        {
            carry = value >> 31;
            value = carry ? 0xFFFFFFFF : 0;
        }
    }
    else if (immediate)
    {
        carry = value >> 31;
        value = carry ? 0xFFFFFFFF : 0;
    }
    return value;
}

u32 ARM::ror(u32 value, int amount, bool& carry, bool immediate) const
{
    if (amount != 0)
    {
        value = rotateRight(value, amount);
        carry = value >> 31;
    }
    else if (immediate)
    {
        carry = value & 0x1;
        value = (cpsr.c << 31) | (value >> 1);
    }
    return value;
}

u32 ARM::shift(Shift type, u32 value, int amount, bool& carry, bool immediate) const
{
    switch (type)
    {
    case Shift::LSL: return lsl(value, amount, carry);
    case Shift::LSR: return lsr(value, amount, carry, immediate);
    case Shift::ASR: return asr(value, amount, carry, immediate);
    case Shift::ROR: return ror(value, amount, carry, immediate);

    default:
        EGG_UNREACHABLE;
        return value;
    }
}

u32 ARM::lsl(u32 value, int amount) const
{
    if (amount != 0)
    {
        if (amount < 32)
            value <<= amount;
        else
            value = 0;
    }
    return value;
}

u32 ARM::lsr(u32 value, int amount, bool immediate) const
{
    if (amount != 0)
    {
        if (amount < 32)
            value >>= amount;
        else
            value = 0;
    }
    else if (immediate)
    {
        value = 0;
    }
    return value;
}

u32 ARM::asr(u32 value, int amount, bool immediate) const
{
    if (amount != 0)
    {
        value = static_cast<s32>(value) >> std::max(amount, 31);
    }
    else if (immediate)
    {
        value = static_cast<s32>(value) >> 31;
    }
    return value;
}

u32 ARM::ror(u32 value, int amount, bool immediate) const
{
    if (amount != 0)
    {
        value = rotateRight(value, amount);
    }
    else if (immediate)
    {
        value = (cpsr.c << 31) | (value >> 1);
    }
    return value;
}

u32 ARM::shift(Shift type, u32 value, int amount, bool immediate) const
{
    switch (type)
    {
    case Shift::LSL: return lsl(value, amount);
    case Shift::LSR: return lsr(value, amount, immediate);
    case Shift::ASR: return asr(value, amount, immediate);
    case Shift::ROR: return ror(value, amount, immediate);

    default:
        EGG_UNREACHABLE;
        return value;
    }
}
