#pragma once

#include <memory>
#include <string>
#include <vector>
 
#include "backup.h"
#include "common/fs.h"

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

    bool load(const Path& file);

    std::size_t size() const;

    std::unique_ptr<Backup> backup;

private:
    static Path toBackupFile(const Path& file);
    static std::string makeString(u8* data, int size);
    static u32 readUnused(u32 addr);

    Header parseHeader();
    Backup::Type parseBackupType();

    Path file;
    std::vector<u8> data;
};
