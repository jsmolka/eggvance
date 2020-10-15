#pragma once

#include "mmu/ram.h"

class Pram : public Ram<0x400>
{
public:
    void writeByte(u32 addr, u8 byte);

    u16 colorFG(uint index, uint bank = 0) const;
    u16 colorBG(uint index, uint bank = 0) const;
    u16 colorFGOpaque(uint index, uint bank = 0) const;
    u16 colorBGOpaque(uint index, uint bank = 0) const;
    u16 backdrop() const;
};
