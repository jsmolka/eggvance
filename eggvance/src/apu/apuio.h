#pragma once

#include "common/register.h"

struct APUIO
{
    RegisterRW<2> soundcnt1_l;
    RegisterRW<2> soundcnt1_h;
    RegisterRW<2> soundcnt1_x;
    RegisterRW<2> soundcnt2_l;
    RegisterRW<2> soundcnt2_h;
    RegisterRW<2> soundcnt3_l;
    RegisterRW<2> soundcnt3_h;
    RegisterRW<2> soundcnt3_x;
    RegisterRW<2> soundcnt4_l;
    RegisterRW<2> soundcnt4_h;
    RegisterRW<2> soundcnt_l;
    RegisterRW<2> soundcnt_h;
    RegisterRW<2> soundcnt_x;
    RegisterRW<2> soundbias;
    RegisterRW<2> wave_ram[8];

    RegisterW<4> fifo_a;
    RegisterW<4> fifo_b;
};
