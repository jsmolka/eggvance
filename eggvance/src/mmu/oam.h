#pragma once

#include "ram.h"
#include "gpu/oamentry.h"
#include "gpu/matrix.h"

class Oam : public Ram<0x400>
{
public:
    void writeByte(u32 addr, u8  byte) = delete;
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    Matrix matrices[32];
    OamEntry entries[128];
};
