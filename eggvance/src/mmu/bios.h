#pragma once

#include "ram.h"
#include "base/filesystem.h"

class Bios
{
public:
    static constexpr uint kSize = 0x4000;

    static void init(const fs::path& path);

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr);

private:
    static inline Ram<kSize> data;

    u32 previous = 0xE129'F000;
};
