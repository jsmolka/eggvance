#pragma once

#include "register.h"

class IntrRequest : public Register<IntrRequest, 2>
{
public:
    inline IntrRequest& operator|=(u16 value)
    {
        *reinterpret_cast<u16*>(data) |= value;

        return *this;
    }

    inline operator u16()
    {
        return *reinterpret_cast<u16*>(data);
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] &= ~byte;
    }
};
