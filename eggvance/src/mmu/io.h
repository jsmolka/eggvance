#pragma once

#include "common/ram.h"

class IO
{
public:
    IO();

    void reset();

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

private:
    RAM<0x400> data;

    u8 masks[1024];
    u8 memcontrol[4];
};
