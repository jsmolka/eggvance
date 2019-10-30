#pragma once

#include "common/ram.h"

class Palette : public RAM<0x400>
{
public:
    enum class Format { F16, F256 };

    void reset();

    void writeByte(u32 addr, u8 byte);

    u16 colorFG(int index, int bank = 0);
    u16 colorBG(int index, int bank = 0);
    u16 colorFGOpaque(int index, int bank = 0);
    u16 colorBGOpaque(int index, int bank = 0);
    u16 backdrop();
};
