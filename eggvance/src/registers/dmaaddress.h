#pragma once

#include "common/integer.h"

class DMAAddress
{
public:
    inline operator u32() const
    {
        return value;
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 4);

        reinterpret_cast<u8*>(&value)[index] = byte;
    }

    u32 value = 0;
};
