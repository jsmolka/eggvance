#pragma once

#include <string>

#include "common/ram.h"

class BIOS
{
public:
    void reset();

    bool init(const std::string& file);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

private:
    bool read(const std::string& file);
    static u64 hash(u32* data, int size);

    u32 last_fetched;
    RAM<0x4000> data;
};
