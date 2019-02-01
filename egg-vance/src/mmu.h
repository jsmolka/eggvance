#pragma once

#include <vector>

#include "integer.h"

class MMU
{
public:
    MMU();

    void reset();

    u8 read8(u32 address) const;
    u16 read16(u32 address) const;
    u32 read32(u32 address) const;

    void write8(u32 address, u8 value);
    void write16(u32 address, u16 value);
    void write32(u32 address, u32 value);

private:
    std::vector<u8> memory_map;
};

