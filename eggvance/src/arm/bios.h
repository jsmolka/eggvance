#pragma once

#include <array>

#include "base/int.h"
#include "base/filesystem.h"

class Bios
{
public:
    static void init(const fs::path& path);

    template<typename Integral>
    Integral read(u32 addr)
    {
        addr &= ~(sizeof(Integral) - 1);

        if (arm.pc < kSize)
            previous = *reinterpret_cast<u32*>(&data[addr & ~0x3]);

        return previous >> (8 * (addr & 0x3));
    }

    static constexpr uint kSize = 0x4000;

private:
    static std::array<u8, kSize> data;

    u32 previous = 0xE129'F000;
};
