#pragma once

#include <vector>

#include "base/fs.h"
#include "base/integer.h"

class Save
{
public:
    enum class Type
    {
        None,
        Sram,
        Eeprom,
        Flash64,
        Flash128
    };

    Save();
    Save(const fs::path& file, Type type);
    virtual ~Save();

    static Type parse(const std::vector<u8>& rom);

    virtual u8 readByte(u32 addr);
    virtual void writeByte(u32 addr, u8 byte);

    Type type;
    fs::path file;
    std::vector<u8> data;
};
