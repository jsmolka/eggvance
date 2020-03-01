#pragma once

#include "common/integer.h"

class BGOffset
{
public:
    inline operator u16() const
    {
        return value;
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        reinterpret_cast<u8*>(&value)[index] = byte;
    }

    u16 value = 0;
};
