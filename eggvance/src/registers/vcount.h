#pragma once

#include "register.h"

class VCount : public TRegister<VCount, 2>
{
public:
    inline operator uint() const
    {
        return data[0];
    }

    template<uint index>
    inline void write(u8 byte) = delete;

    inline void next()
    {
        data[0] = (data[0] + 1) % 228;
    }
};
