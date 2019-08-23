#pragma once

#include "common/utility.h"

struct BackgroundOffset
{
    template<unsigned index>
    inline void write(u8 byte);

    union
    {
        u8  offset_b[2];  // Offset bytes
        u16 offset;       // Offset (0..511)
    };
};

template<unsigned index>
inline void BackgroundOffset::write(u8 byte)
{
    static_assert(index <= 1);

    if (index == 1)
        byte &= 0x1;

    bytes(&offset)[index] = byte;
}
