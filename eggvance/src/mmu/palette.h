#pragma once

#include "common/ram.h"

class Palette
{
public:
    enum class Format
    {
        F16,
        F256
    };

    static constexpr u32 transparent = 0x8000;

    void reset();

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    u16 readColorFG(int index, int bank = 0);
    u16 readColorBG(int index, int bank = 0);

private:
    RAM<0x400> data;
};
