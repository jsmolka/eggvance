#pragma once

#include "common/utility.h"

struct BackgroundReference
{
    template<unsigned index>
    inline void write(u8 byte);

    union
    {
        u8  ref_b[4];  // Reference point bytes
        s32 ref;       // Reference point
    };
    int internal;  // Internal register (ref copied during V-Blank or write)
};

template<unsigned index>
inline void BackgroundReference::write(u8 byte)
{
    static_assert(index <= 3);

    if (index == 3)
        byte = signExtend<4>(byte);

    bytes(&ref)[index] = byte;
    internal = ref;
}
