#pragma once

#include "base/register.h"

class Core;

class Io
{
public:
    Io(Core& core);

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

private:
    Core& core;

    struct UnusedIo
    {
        UnusedIo();

        Register<2, 0x007F> soundcnt1_l;
        Register<2, 0xFFC0> soundcnt1_h;
        Register<2, 0x4000> soundcnt1_x;
        Register<2, 0xFFC0> soundcnt2_l;
        Register<2, 0x4000> soundcnt2_h;
        Register<2, 0x00E0> soundcnt3_l;
        Register<2, 0xE000> soundcnt3_h;
        Register<2, 0x4000> soundcnt3_x;
        Register<2, 0xFF00> soundcnt4_l;
        Register<2, 0x40FF> soundcnt4_h;
        Register<2, 0xFF77> soundcnt_l;
        Register<2, 0x770F> soundcnt_h;
        Register<2, 0x0080> soundcnt_x;
        Register<2> soundbias;
        Register<2> waveram[8];
        RegisterW<4> fifo[2];

        Register<2> rcnt;
        Register<2> joycnt;
        Register<4> joyrecv;
        Register<4> joytrans;
        Register<2> joystat;
        Register<2> siocnt;
        Register<2> siomulti[4];
        Register<2> siosend;
        Register<1> postflag;
    }
    unused;
};
