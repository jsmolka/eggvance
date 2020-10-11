#pragma once

#include "ram.h"
#include "base/int.h"
#include "base/filesystem.h"

class BIOS
{
public:
    void reset();

    void init(const fs::path& path);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

private:
    u32 readProtected(u32 addr) const;

    bool read(const fs::path& path);
    static u64 hash(u32* data, int size);

    u32 last_fetched;
    Ram<0x4000> data;
};
