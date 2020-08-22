#pragma once

#include <array>

#include "ram.h"
#include "base/eggcpt.h"

class Core;

class BIOS
{
public:
    BIOS(Core& core);

    void reset();

    void init(const fs::path& path);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

private:
    Core& core;

    u32 readProtected(u32 addr) const;

    bool read(const fs::path& path);
    static u64 hash(u32* data, int size);

    u32 last_fetched;
    MirroredRAM<0x4000> data;
    static std::array<u8, 0x4000> replacement_bios;
};
