#pragma once

#include "integer.h"

template<unsigned int bit>
inline s64 twos(u64 value)
{
    if (value & 1 << bit)
    {
        u64 mask = (1 << (bit + 1)) - 1;
        return -1 * ((~value & mask) + 1);
    }
    return value;
}
