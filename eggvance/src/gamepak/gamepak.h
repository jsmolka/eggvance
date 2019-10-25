#pragma once

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
    static std::string toBackupFile(const std::string& file);
    static std::string makeString(u8* data, int size);
    static u32 readUnused(u32 addr);

    Header parseHeader();
    Backup::Type parseBackupType();

    std::string file;
    std::vector<u8> data;
};
