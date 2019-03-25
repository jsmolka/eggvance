#pragma once

#include "integer.h"

// Align address for halfword access
inline void align_half(u32& addr)
{
    addr &= ~0x1;
}

// Align address for word access
inline void align_word(u32& addr)
{
    addr &= ~0x3;
}

// Convert two's complement to signed value
template<unsigned int bits>
inline s32 twos(u32 value)
{
    static_assert(bits <= 32, "Invalid number of bits");

    // Check if sign bit is set
    if (value & 1 << (bits - 1))
    {
        u32 mask = (1 << bits) - 1;
        return -static_cast<s32>((~value + 1) & mask);
    }
    return value;
}

template<>
inline s32 twos<32>(u32 value)
{
    return static_cast<s32>(value);
}
