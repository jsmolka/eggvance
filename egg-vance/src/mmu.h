#pragma once

#include <string>
#include <vector>

#include "integer.h"
#include "lcd_stat.h"

class MMU
{
public:
    MMU();

    void reset();

    bool loadRom(const std::string& filepath);

    u8 readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void writeByteFast(u32 addr, u8 byte);
    void writeHalfFast(u32 addr, u16 half);
    void writeWordFast(u32 addr, u32 word);

    void writeByte(u32 addr, u8 byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    lcd_stat* lcd_stat;

private:
    std::vector<u8> memory;
};

