#pragma once

#include <vector>
#include <string>

#include "common/integer.h"

class Backup
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

    Backup();
    Backup(const std::string& file, Type type);
    virtual ~Backup();

    virtual u8 readByte(u32 addr);
    virtual void writeByte(u32 addr, u8 byte);

    Type type;
    std::vector<u8> data;

private:
    std::string file;
};
