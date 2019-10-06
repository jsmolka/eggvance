#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct WaitControl
{
    struct WaitState
    {
        void reset();

        int n;  // Nonsequential cycles (0..3 = 4,3,2,8 cycles)
        int s;  // Sequential cycles (dependent on waitstate)
    };

    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    int sram;       // SRAM wait control (0..3 = 4,3,2,8 cycles)
    WaitState ws0;  // Sequential cycles (0..1 = 2,1 cycles)
    WaitState ws1;  // Sequential cycles (0..1 = 4,1 cycles)
    WaitState ws2;  // Sequential cycles (0..1 = 8,1 cycles)
};

template<unsigned index>
inline void WaitControl::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0:
        sram  = bits<0, 2>(byte);
        ws0.n = bits<2, 2>(byte);
        ws0.s = bits<4, 1>(byte);
        ws1.n = bits<5, 2>(byte);
        ws1.s = bits<7, 1>(byte);
        break;

    case 1:
        ws2.n = bits<0, 2>(byte);
        ws2.s = bits<2, 1>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
