#pragma once

#include <memory>
#include <string>
#include <vector>

#include "backup.h"

class Cartridge
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

    bool load(const std::string& path);

    std::size_t size() const;

    std::unique_ptr<Backup> backup;

private:
    bool FileRead(const std::string& path, std::vector<u8>& dst);
    bool FileWrite(const std::string& path, std::vector<u8>& src);

    std::string path;
    std::vector<u8> data;
};
