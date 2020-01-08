#pragma once

#include <array>

#include "ram.h"
#include "common/fs.h"

class BIOS
{
public:
    void reset();

    void init(const Path& file);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

private:
    u32 readProtected(u32 addr) const;

    bool read(const Path& file);
    static u64 hash(u32* data, int size);

    u32 last_fetched;
    RAM<0x4000> data;
    static std::array<u8, 0x4000> replacement_bios;
};
