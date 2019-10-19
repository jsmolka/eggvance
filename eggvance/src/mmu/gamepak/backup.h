#pragma once

#include <vector>

#include "common/integer.h"

class Backup
{
public:
    u8 readByte(u32 addr) { return 0; };
    void writeByte(u32 addr, u8 byte) { };

    enum class Type
    {
        EEPROM,
        SRAM,
        FLASH64,
        FLASH128
    } type;

    std::vector<u8> data;
};
