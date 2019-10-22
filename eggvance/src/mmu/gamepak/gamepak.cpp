#include "gamepak.h"

#include <cstring>
#include <filesystem>

#include "eeprom.h"
#include "flash.h"
#include "fileutil.h"
#include "sram.h"

namespace fs = std::filesystem;

u8 GamePak::readByte(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<u8*>(&data[addr]);
    else
        return 0;
}

u16 GamePak::readHalf(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<u16*>(&data[addr]);
    else
        return 0;
}

u32 GamePak::readWord(u32 addr)
{
    if (addr < data.size())
        return *reinterpret_cast<u32*>(&data[addr]);
    else
        return 0;
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

        case Backup::Type::FLASH64:
        case Backup::Type::FLASH128:
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
    fs::path path = file;
    path.replace_extension(".sav");
    return path.string();
}

std::string GamePak::makeString(u8* data, int size)
{
    std::string str(reinterpret_cast<char*>(data), size);
    str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
    return str;
}

GamePak::Header GamePak::parseHeader()
{
    return Header {
        .title = makeString(&data[0xA0], 12),
        .maker = makeString(&data[0xB0], 2),
        .code  = makeString(&data[0xAC], 4)
    };
}

Backup::Type GamePak::parseBackupType()
{
    static const std::pair<std::string, Backup::Type> backup_types[5] = {
        { "SRAM",      Backup::Type::SRAM     },
        { "EEPROM",    Backup::Type::EEPROM   },
        { "FLASH_",    Backup::Type::FLASH64  },
        { "FLASH512_", Backup::Type::FLASH64  },
        { "FLASH1M_",  Backup::Type::FLASH128 }
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
    return Backup::Type::NONE;
}
