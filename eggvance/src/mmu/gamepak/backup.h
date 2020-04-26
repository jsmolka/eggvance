#pragma once

#include <vector>

#include "base/fs.h"
#include "base/integer.h"

class Backup
{
public:
    enum class Type
    {
        None,
        SRAM,
        EEPROM,
        Flash64,
        Flash128
    };

    Backup();
    Backup(const fs::path& file, Type type);
    virtual ~Backup();

    virtual u8 readByte(u32 addr);
    virtual void writeByte(u32 addr, u8 byte);

    Type type;
    fs::path file;
    std::vector<u8> data;
};
