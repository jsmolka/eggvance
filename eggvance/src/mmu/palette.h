#pragma once

#include "ram.h"

class Palette : public RAM<0x400>
{
public:
    enum class Format { F16, F256 };

    void reset();

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    u16 colorFG(int index, int bank = 0) const;
    u16 colorBG(int index, int bank = 0) const;
    u16 colorFGOpaque(int index, int bank = 0) const;
    u16 colorBGOpaque(int index, int bank = 0) const;
    u16 backdrop() const;

private:
    u16 colors[0x200];
};
