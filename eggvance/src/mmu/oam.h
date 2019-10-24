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

private:
    OAMEntry entries[128];
};
