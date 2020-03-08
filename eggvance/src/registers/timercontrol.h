#pragma once

#include "register.h"
#include "common/bits.h"
#include "common/macros.h"

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
            {
                prescaler = 1;
            }
            else
            {
                switch (bits<0, 2>(byte))
                {
                case 0: prescaler = 1; break;
                case 1: prescaler = 64; break;
                case 2: prescaler = 256; break;
                case 3: prescaler = 1024; break;

                default:
                    UNREACHABLE;
                    break;
                }
            }
        }
    }

    uint prescaler = 1;
    uint cascade   = 0;
    uint irq       = 0;
    uint enabled   = 0;
};
