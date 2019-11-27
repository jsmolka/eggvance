#pragma once

#include <array>

#include "ppu/oamentry.h"
#include "ram.h"

class OAM : public RAM<0x400>
{
public:
    void reset();

    void writeByte(u32 addr, u8  byte) = delete;
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    s16 pa(int parameter);
    s16 pb(int parameter);
    s16 pc(int parameter);
    s16 pd(int parameter);

    std::array<OAMEntry, 128> entries;
};
