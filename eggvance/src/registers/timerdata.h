#pragma once

#include "register.h"

class TimerData : public Register<TimerData, 2>
{
public:
    template<uint index>
    inline u8 read()
    {
        static_assert(index < 2);

        return reinterpret_cast<u8*>(&counter)[index];
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        reinterpret_cast<u8*>(&reload)[index] = byte;
    }

    u16 counter = 0;
    u16 reload  = 0;
};
