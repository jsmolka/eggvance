#pragma once

#include "gpu/oamentry.h"
#include "gpu/matrix.h"
#include "ram.h"

class Oam : public Ram<0x400>
{
public:
    void writeByte(u32 addr, u8  byte) = delete;
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    Matrix matrix(uint index) const;

    OamEntry entries[128];
};
