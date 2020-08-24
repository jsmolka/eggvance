#pragma once

#include <vector>

#include "base/int.h"
#include "base/filesystem.h"

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

    virtual u8 read(u32 addr);
    virtual void write(u32 addr, u8 byte);

    const Type type;
    const fs::path file;
    std::vector<u8> data;
};
