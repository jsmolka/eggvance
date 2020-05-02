#include "gamepak.h"

#include <cstring>

#include "eeprom.h"
#include "flash.h"
#include "sram.h"
#include "base/config.h"

struct BackupConfig
{
    std::string id;
    Backup::Type type;
};

static const BackupConfig backup_configs[5] =
{
    { "SRAM"     , Backup::Type::SRAM     },
    { "EEPROM"   , Backup::Type::EEPROM   },
    { "FLASH_"   , Backup::Type::Flash64  },
    { "FLASH512_", Backup::Type::Flash64  },
    { "FLASH1M_" , Backup::Type::Flash128 }
};

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

bool GamePak::load(const fs::path& file, const fs::path& backup)
{
    if (!fs::read(file, data))
        return false;

    initHeader();
    initBackup(backup, backupType());

    return true;
}

bool GamePak::load(const fs::path& file)
{
    auto backup = fs::path(file).replace_extension("sav");
    if (!config.save_path.empty())
        backup = config.save_path / backup.filename();

    return load(file, backup);
}

bool GamePak::loadBackup(const fs::path& file)
{
    initBackup(file, backup->type);

    return fs::read(file, backup->data);;
}

std::size_t GamePak::size() const
{
    return data.size();
}

u32 GamePak::readUnused(u32 addr)
{
    addr = (addr & ~0x3) >> 1;
    return (addr & 0xFFFF) | ((addr + 1) & 0xFFFF) << 16;
}

void GamePak::initHeader()
{
    auto toString = [](u8* data, uint size)
    {
        std::string str(reinterpret_cast<char*>(data), size);
        str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
        return str;
    };

    header.title = toString(&data[0xA0], 12);
    header.maker = toString(&data[0xB0],  2);
    header.code  = toString(&data[0xAC],  4);
}

void GamePak::initBackup(const fs::path& file, Backup::Type type)
{
    backup = nullptr;

    switch (type)
    {
    case Backup::Type::SRAM:
        backup = std::make_unique<SRAM>(file);
        break;

    case Backup::Type::EEPROM:
        backup = std::make_unique<EEPROM>(file);
        break;

    case Backup::Type::Flash64:
    case Backup::Type::Flash128:
        backup = std::make_unique<Flash>(file, type);
        break;

    default:
        backup = std::make_unique<Backup>();
        break;
    }
}

Backup::Type GamePak::backupType() const
{
    for (uint x = 0xC0; x < data.size(); x += 4)
    {
        for (const auto& config : backup_configs)
        {
            if (std::memcmp(config.id.data(), &data[x], config.id.size()) == 0)
                return config.type;
        }
    }
    return Backup::Type::None;
}
