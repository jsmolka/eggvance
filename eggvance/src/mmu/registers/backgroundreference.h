#pragma once

#include "common/utility.h"

struct BackgroundReference
{
    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    int reference;  // Reference point
    int internal;   // Internal register (reference copied during V-Blank or write)
};

template<unsigned index>
inline void BackgroundReference::write(u8 byte)
{
    static_assert(index <= 3);

    if (index == 3)
        byte = signExtend<4>(byte);

    bytes(&reference)[index] = byte;
    internal = reference;
}
