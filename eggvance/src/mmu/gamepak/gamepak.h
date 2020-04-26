#pragma once

#include <memory>
#include <string>
#include <vector>
 
#include "backup.h"
#include "base/fs.h"

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

    bool load(const fs::path& file, const fs::path& backup);
    bool load(const fs::path& file);
    bool loadBackup(const fs::path& file);

    std::size_t size() const;

    std::unique_ptr<Backup> backup;

private:
    static u32 readUnused(u32 addr);

    void initHeader();
    void initBackup(const fs::path& file, Backup::Type type);

    Backup::Type backupType() const;

    fs::path file;
    std::vector<u8> data;
};
