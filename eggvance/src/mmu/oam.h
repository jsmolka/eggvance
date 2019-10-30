#pragma once

#include "common/ram.h"
#include "oamentry.h"

class OAM : public RAM<0x400>
{
public:
    void reset();

    const OAMEntry& entry(int index) const;

    void writeByte(u32 addr, u8  byte) = delete;
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    s16 pa(int parameter);
    s16 pb(int parameter);
    s16 pc(int parameter);
    s16 pd(int parameter);

private:
    OAMEntry entries[128];
};
