#include "gamepak.h"

#include <cstring>
#include <filesystem>
#include <fmt/format.h>

#include "eeprom.h"
#include "flash.h"
#include "sram.h"
#include "common/config.h"
#include "common/fileutil.h"

namespace fs = std::filesystem;

u8 GamePak::readByte(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<u8*>(&data[addr]);
    else
        return readUnused(addr);
}

u16 GamePak::readHalf(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<u16*>(&data[addr]);
    else
        return readUnused(addr);
}

u32 GamePak::readWord(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<u32*>(&data[addr]);
    else
        return readUnused(addr);
}

bool GamePak::load(const std::string& file)
{
    if (this->file != file)
    {
        if (!fileutil::read(file, data))
            return false;

        header = parseHeader();

        auto backup_file = toBackupFile(file);
        auto backup_type = parseBackupType();

        switch (backup_type)
        {
        case Backup::Type::SRAM:
            backup = std::make_unique<SRAM>(backup_file);
            break;

        case Backup::Type::EEPROM:
            backup = std::make_unique<EEPROM>(backup_file);
            break;

        case Backup::Type::Flash64:
        case Backup::Type::Flash128:
            backup = std::make_unique<Flash>(backup_file, backup_type);
            break;

        default:
            backup = std::make_unique<Backup>();
            break;
        }
    }
    return true;
}

std::size_t GamePak::size() const
{
    return data.size();
}

std::string GamePak::toBackupFile(const std::string& file)
{
    fs::path path(file);

    if (config.save_dir.empty())
        return path.replace_extension(".sav").string();
    else
        return fileutil::concat(config.save_dir, path.filename().replace_extension(".sav").string());
}

std::string GamePak::makeString(u8* data, int size)
{
    std::string str(reinterpret_cast<char*>(data), size);
    str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
    return str;
}

u32 GamePak::readUnused(u32 addr)
{
    addr = (addr & ~0x3) >> 1;
    return (addr & 0xFFFF) | ((addr + 1) & 0xFFFF) << 16;
}

GamePak::Header GamePak::parseHeader()
{
    return {
        makeString(&data[0xA0], 12),
        makeString(&data[0xB0], 2),
        makeString(&data[0xAC], 4)
    };
}

Backup::Type GamePak::parseBackupType()
{
    static const std::pair<std::string, Backup::Type> backup_types[5] = {
        { "SRAM",      Backup::Type::SRAM     },
        { "EEPROM",    Backup::Type::EEPROM   },
        { "FLASH_",    Backup::Type::Flash64  },
        { "FLASH512_", Backup::Type::Flash64  },
        { "FLASH1M_",  Backup::Type::Flash128 }
    };

    std::string id;
    Backup::Type type;

    for (int x = 0xC0; x < data.size(); x += 4)
    {
        for (const auto& save_type : backup_types)
        {
            std::tie(id, type) = save_type;
            if (std::memcmp(id.data(), &data[x], id.size()) == 0)
                return type;
        }
    }
    return Backup::Type::None;
}
