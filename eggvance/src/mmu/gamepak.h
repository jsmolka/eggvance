#pragma once

#include <string>
#include <vector>

#include "ram.h"

class GamePak
{
public:
    enum class Save
    {
        NONE,
        SRAM,
        FLASH,
        EEPROM
    };

    struct Header
    {
        std::string title;
        std::string code;
        std::string maker;
    };

    GamePak(const std::string& file);

    u8 readByte(u32 addr) const;

    u8 readSaveByte(u32 addr) const;
    void writeSaveByte(u32 addr, u8 byte);

    bool valid;
    Header header;

private:
    static std::string makeString(u8* data, int size);

    bool read(const std::string& file);
    void parseHeader();

    std::vector<u8> data;
    RAM<0x10000> sram;
};

