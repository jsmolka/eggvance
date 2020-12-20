#pragma once

#include "matrix.h"
#include "oamentry.h"
#include "mmu/ram.h"

class Oam : public Ram<0x400>
{
public:
    void writeByte(u32 addr, u8  byte) = delete;
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    Matrix matrices[32];
    OamEntry entries[128];
};
