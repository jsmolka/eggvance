#include "gamepak.h"

#include <cstring>
#include <filesystem>

#include "dummy.h"
#include "eeprom.h"
#include "flash.h"
#include "file.h"
#include "sram.h"

namespace fs = std::filesystem;

GamePak::GamePak(const std::string& file)
{
    if (valid = File::read(file, data))
    {
        parseHeader();

        Save::Type save_type = parseSaveType();
        std::string save_file = toSaveFile(file);
        switch (save_type)
        {
        case Save::Type::SRAM:
            save = std::make_unique<SRAM>(save_file);
            break;

        case Save::Type::EEPROM:
            save = std::make_unique<EEPROM>(save_file);
            break;

        case Save::Type::FLASH64:
        case Save::Type::FLASH128:
            save = std::make_unique<Flash>(save_file, save_type);
            break;

        default:
            save = std::make_unique<Dummy>();
            break;
        }
    }
}

u8 GamePak::readByte(u32 addr) const
{
    return addr < data.size() ? data[addr] : 0;
}

std::size_t GamePak::size() const
{
    return data.size();
}

std::string GamePak::toSaveFile(const std::string& file)
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

void GamePak::parseHeader()
{
    header.title = makeString(&data[0xA0], 12);
    header.code  = makeString(&data[0xAC], 4);
    header.maker = makeString(&data[0xB0], 2);
}

Save::Type GamePak::parseSaveType()
{
    static const std::pair<std::string, Save::Type> save_types[5] = {
        { "SRAM",      Save::Type::SRAM     },
        { "EEPROM",    Save::Type::EEPROM   },
        { "FLASH_",    Save::Type::FLASH64  },
        { "FLASH512_", Save::Type::FLASH64  },
        { "FLASH1M_",  Save::Type::FLASH128 }
    };

    std::string id;
    Save::Type type;
    for (int x = 0xC0; x < data.size(); x += 4)
    {
        u8* ptr = &data[x];
        for (const auto& save_type : save_types)
        {
            std::tie(id, type) = save_type;
            if (std::memcmp(id.data(), ptr, id.size()) == 0)
                return type;
        }
    }
    return Save::Type::NONE;
}
