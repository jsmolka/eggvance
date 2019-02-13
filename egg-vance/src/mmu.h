#pragma once

#include <string>
#include <vector>

#include "integer.h"
#include "internal.h"
#include "lcdstat.h"

class Mmu : public Internal
{
public:
    Mmu();

    void reset() final;

    bool loadRom(const std::string& file);

    u8 readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void writeByte(u32 addr, u8 byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    LcdStat* lcd_stat;

private:
    std::vector<u8> memory;
};
