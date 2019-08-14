#pragma once

#include <array>
#include <string>

#include "ram.h"

class BIOS
{
public:
    BIOS(const std::string& file);

    u8 readByte(u32 addr) const;

    bool valid;

private:
    bool read(const std::string& file);
    static u64 hash(u32* data, int size);

    RAM<0x4000> data;
};
