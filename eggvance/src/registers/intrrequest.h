#pragma once

#include "common/integer.h"

class IntrRequest
{
public:
    IntrRequest();

    void reset();

    template<uint index>
    inline u8 read() const
    {
        static_assert(index <= 1);

        return request >> (8 * index);
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index <= 1);

        reinterpret_cast<u8*>(&request)[index] &= ~byte;
    }

    inline operator uint() const
    {
        return request;
    }

    inline IntrRequest& operator|=(uint value)
    {
        request |= value;

        return *this;
    }

    uint request;
};
