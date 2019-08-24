#pragma once

#include "common/utility.h"

struct BackgroundOffset
{
    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    int offset;  // Offset (0..511)
};

template<unsigned index>
inline void BackgroundOffset::write(u8 byte)
{
    static_assert(index <= 1);

    if (index == 1)
        byte &= 0x1;

    bytes(&offset)[index] = byte;
}
