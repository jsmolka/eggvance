#pragma once

#include "register.h"
#include "common/bits.h"
#include "common/utility.h"

class TimerControl : public RegisterRW<2>
{
public:
    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            cascade = bits<2, 1>(byte);
            irq     = bits<6, 1>(byte);
            enabled = bits<7, 1>(byte);

            if (cascade)
                prescaler = 1;
            else
                prescaler = stacklut<1, 64, 256, 1024>(bits<0, 2>(byte));
        }
    }

    uint prescaler = 1;
    uint cascade   = 0;
    uint irq       = 0;
    uint enabled   = 0;
};
