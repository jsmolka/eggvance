#pragma once

#include <array>

#include "base/filesystem.h"
#include "base/int.h"

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

    static std::array<u8, kSize> data;

    u32 previous = 0xE129'F000;
};
