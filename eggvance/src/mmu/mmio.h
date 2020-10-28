#pragma once

#include "base/register.h"

class Mmio
{
public:
    Mmio();

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

private:
    Register<u16, 0x007F> soundcnt1_l;
    Register<u16, 0xFFC0> soundcnt1_h;
    Register<u16, 0x4000> soundcnt1_x;
    Register<u16, 0xFFC0> soundcnt2_l;
    Register<u16, 0x4000> soundcnt2_h;
    Register<u16, 0x00E0> soundcnt3_l;
    Register<u16, 0xE000> soundcnt3_h;
    Register<u16, 0x4000> soundcnt3_x;
    Register<u16, 0xFF00> soundcnt4_l;
    Register<u16, 0x40FF> soundcnt4_h;
    Register<u16, 0xFF77> soundcnt_l;
    Register<u16, 0x770F> soundcnt_h;
    Register<u16, 0x0080> soundcnt_x;
    Register<u16> soundbias;
    Register<u16> waveram[8];
    RegisterW<u32> fifo[2];

    Register<u16> rcnt;
    Register<u16> joycnt;
    Register<u32> joyrecv;
    Register<u32> joytrans;
    Register<u16> joystat;
    Register<u16> siocnt;
    Register<u16> siomulti[4];
    Register<u16> siosend;
    Register<u8>  postflag;
};
