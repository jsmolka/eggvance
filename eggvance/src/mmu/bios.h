#pragma once

#include <array>
#include <string>

#include "ram.h"

class BIOS
{
public:
    BIOS(const std::string& file);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    bool valid;

private:
    template<typename T>
    T read(u32 addr);

    bool readFile(const std::string& file);
    static u64 hash(u32* data, int size);

    RAM<0x4000> data;
};
