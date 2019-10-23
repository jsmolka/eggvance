#pragma once

#include "common/ram.h"
#include "oamentry.h"

class OAM
{
public:
    void reset();

    const OAMEntry& entry(int index) const;

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

private:
    OAMEntry entries[128];

    RAM<0x400> data;
};
