#pragma once

#include "register.h"

class IntrRequest : public RegisterRW<2>
{
public:
    inline IntrRequest& operator|=(u16 value)
    {
        cast<u16>() |= value;

        return *this;
    }

    inline operator u16()
    {
        return cast<u16>();
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] &= ~byte;
    }
};
