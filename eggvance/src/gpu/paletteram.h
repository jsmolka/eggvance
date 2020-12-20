#pragma once

#include "base/ram.h"

class PaletteRam : public Ram<0x400>
{
public:
    void writeByte(u32 addr, u8 byte);

    u16 colorFg(uint index, uint bank = 0) const;
    u16 colorBg(uint index, uint bank = 0) const;
    u16 colorFgOpaque(uint index, uint bank = 0) const;
    u16 colorBgOpaque(uint index, uint bank = 0) const;
    u16 backdrop() const;
};
