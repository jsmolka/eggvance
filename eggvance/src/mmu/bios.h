#pragma once

#include "ram.h"
#include "base/int.h"
#include "base/filesystem.h"

class Bios
{
public:
    static constexpr uint kSize = 0x4000;

    void reset();

    void init(const fs::path& path);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

private:
    u32 previous;
    Ram<kSize> data;
};
