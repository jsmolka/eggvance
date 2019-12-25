#pragma once

#include <array>

#include "ram.h"
#include "ppu/oamentry.h"
#include "ppu/matrix.h"

class OAM : public RAM<0x400>
{
public:
    void reset();

    void writeByte(u32 addr, u8  byte) = delete;
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    Matrix matrix(int index);

    std::array<OAMEntry, 128> entries;
};
