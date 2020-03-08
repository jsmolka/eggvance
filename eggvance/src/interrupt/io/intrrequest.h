#pragma once

#include "common/integer.h"

class IntrRequest
{
public:
    inline IntrRequest& operator|=(u16 value)
    {
        this->value |= value;

        return *this;
    }

    inline operator u16() const
    {
        return value;
    }

    template<uint index>
    inline u8 read() const
    {
        static_assert(index < 2);

        return reinterpret_cast<const u8*>(&value)[index];
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        reinterpret_cast<u8*>(&value)[index] &= ~byte;
    }

    u16 value = 0;
};
