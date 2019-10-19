#pragma once

// Todo:
// - implement backup types
// - test header checksum (CRC)

#include <memory>
#include <string>
#include <vector>
 
#include "backup.h"

class GamePak
{
public:
    struct Header
    {
        std::string title;
        std::string maker;
        std::string code;
    } header;

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    bool load(const std::string& file);

    std::size_t size() const;

    std::unique_ptr<Backup> backup;

private:
    std::string file;
    std::vector<u8> data;
};
