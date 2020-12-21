#pragma once

#include "base/filesystem.h"
#include "base/ram.h"

class Bios
{
public:
    static void init(const fs::path& path);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    static constexpr uint kSize = 0x4000;

private:
    template<typename Integral>
    Integral read(u32 addr);

    static Ram<kSize> data;

    u32 previous = 0xE129'F000;
};
