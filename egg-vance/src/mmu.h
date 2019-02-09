#pragma once

#include <string>
#include <vector>

#include "integer.h"

class MMU
{
public:
    MMU();

    void reset();

    bool loadRom(const std::string& filepath);

    u8 readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void writeByte(u32 addr, u8 byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

private:
    std::vector<u8> memory;
};

