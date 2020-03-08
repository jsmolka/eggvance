#pragma once

#include "common/integer.h"

class TimerData
{
public:
    template<uint index>
    inline u8 read() const
    {
        static_assert(index < 2);

        return reinterpret_cast<const u8*>(&counter)[index];
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        reinterpret_cast<u8*>(&initial)[index] = byte;
    }

    u16 counter = 0;
    u16 initial = 0;
};
