#pragma once

#include "common/integer.h"

class IntrEnable
{
public:
    IntrEnable();

    void reset();

    template<uint index>
    inline u8 read() const
    {
        static_assert(index <= 1);

        return enable >> (8 * index);
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index <= 1);

        reinterpret_cast<u8*>(&enable)[index] = byte;
    }

    inline operator uint() const
    {
        return enable;
    }

    uint enable;
};
