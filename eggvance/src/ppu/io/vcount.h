#pragma once

#include "common/integer.h"

class VCount
{
public:
    inline operator uint() const
    {
        return value;
    }

    template<uint index>
    inline u8 read() const
    {
        static_assert(index < 2);

        return reinterpret_cast<const u8*>(&value)[index];
    }

    inline void next()
    {
        value = (value + 1) % 228;
    }

    u16 value = 0;
};
