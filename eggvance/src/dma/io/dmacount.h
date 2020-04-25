#pragma once

#include "base/bits.h"

class DMACount
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        reinterpret_cast<u8*>(&value)[index] = byte;
    }

    inline uint count(uint id) const
    {
        if (value == 0)
            return id < 3 ? 0x4000 : 0x1'0000;
        else
            return value;
    }

    u16 value = 0;
};
