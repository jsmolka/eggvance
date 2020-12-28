#pragma once

#include "base/register.h"

class Apu
{
public:
    Apu();

    Register<u16, 0x007F> soundcnt1l;
    Register<u16, 0xFFC0> soundcnt1h;
    Register<u16, 0x4000> soundcnt1x;
    Register<u16, 0xFFC0> soundcnt2l;
    Register<u16, 0x4000> soundcnt2h;
    Register<u16, 0x00E0> soundcnt3l;
    Register<u16, 0xE000> soundcnt3h;
    Register<u16, 0x4000> soundcnt3x;
    Register<u16, 0xFF00> soundcnt4l;
    Register<u16, 0x40FF> soundcnt4h;
    Register<u16, 0xFF77> soundcntl;
    Register<u16, 0x770F> soundcnth;
    Register<u16, 0x0080> soundcntx;
    Register<u16> soundbias;
    Register<u16> waveram[8];
    RegisterW<u32> fifo[2];
};

inline Apu apu;