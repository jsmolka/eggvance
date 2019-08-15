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
    ~GamePak();

    u8 readByte(u32 addr) const;

    u8 readSaveByte(u32 addr) const;
    void writeSaveByte(u32 addr, u8 byte);

    bool valid;
    Header header;
    Save save_type;
    std::string save_file;

private:
    bool static read(const std::string& file, std::vector<u8>& dst);
    void static write(const std::string& file, std::vector<u8>& src);

    static std::string makeString(u8* data, int size);
    static bool isSaveType(u8* data, const std::vector<u8>& type);

    void parseHeader();
    void detectSave();

    std::vector<u8> data;
    std::vector<u8> sram;
};

