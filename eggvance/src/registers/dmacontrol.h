#pragma once

#include "register.h"

class DMAControl : public TRegister<DMAControl, 2>
{
public:
    template<uint index>
    inline u8 read() const
    {
        static_assert(index < 2);

        if (index == 0)
            return data[0];
        else
            return (data[1] & ~(1 << 7)) | (enabled << 7);
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 2);

        data[index] = byte;

        if (index == 0)
        {
            dad_delta = bits<5, 2>(byte);
            sad_delta = bits<7, 1>(byte) << 0 | (sad_delta & ~0x1);
        }
        else
        {
            sad_delta = bits<0, 1>(byte) << 1 | (sad_delta & ~0x2);
            repeat    = bits<1, 1>(byte);
            word      = bits<2, 1>(byte);
            timing    = bits<4, 2>(byte);
            irq       = bits<6, 1>(byte);
            reload    = bits<7, 1>(byte) != enabled;
            enabled   = bits<7, 1>(byte);
        }
    }

    uint sad_delta = 0;
    uint dad_delta = 0;
    uint repeat    = 0;
    uint word      = 0;
    uint timing    = 0;
    uint irq       = 0;
    uint enabled   = 0;

    bool reload = false;
};
