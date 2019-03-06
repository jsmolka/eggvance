#pragma once

#include "integer.h"

template<unsigned int bit>
inline s32 twos(u32 value)
{
    if (value & 1 << bit)
    {
        u32 mask = (1 << (bit + 1)) - 1;
        return -1 * ((~value & mask) + 1);
    }
    return value;
}
