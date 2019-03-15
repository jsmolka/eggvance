#pragma once

#include "integer.h"

// Align address for 16-bit value
inline void align16(u32& addr)
{
    addr &= ~0x1;
}

// Align address for 32-bit value
inline void align32(u32& addr)
{
    addr &= ~0x3;
}

// Convert value to two's complement
template<unsigned int bits>
inline s32 twos(u32 value)
{
    // Check if sign bit is set
    if (value & ((u64)1 << (bits - 1)))
    {
        u64 mask = ((u64)1 << bits) - 1;
        return -1 * ((~value + 1) & mask);
    }
    return value;
}
