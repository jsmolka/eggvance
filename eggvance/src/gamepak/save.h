#pragma once

#include <string>
#include <vector>

#include "common/integer.h"

class Save
{
public:
    enum class Type
    {
        NONE,
        SRAM,
        EEPROM,
        FLASH64,
        FLASH128
    };

    Save(const std::string& file, Type type);
    virtual ~Save();

    virtual u8 readByte(u32 addr) = 0;
    virtual void writeByte(u32 addr, u8 byte) = 0;

    Type type;
    std::vector<u8> data;

protected:
    std::string file;
};
