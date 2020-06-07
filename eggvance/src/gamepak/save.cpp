#include "save.h"

#include <algorithm>
#include <cstring>
#include <string_view>
#include <utility>

static constexpr std::pair<std::string_view, Save::Type> save_types[5] =
{
    { "SRAM"     , Save::Type::Sram     },
    { "EEPROM"   , Save::Type::Eeprom   },
    { "FLASH_"   , Save::Type::Flash64  },
    { "FLASH512_", Save::Type::Flash64  },
    { "FLASH1M_" , Save::Type::Flash128 }
};

static constexpr std::size_t maxIdSize = []()
{
    std::size_t max = 0;
    for (const auto& [id, type] : save_types)
        max = std::max(id.size(), max);

    return max;
}();

Save::Save()
    : type(Type::None)
{

}

Save::Save(const fs::path& file, Type type)
    : file(file)
    , type(type)
{
    if (fs::is_regular_file(file))
        fs::read(file, data);
}

Save::~Save()
{
    if (type != Type::None)
        fs::write(file, data);
}

Save::Type Save::parse(const std::vector<u8>& rom)
{
    for (uint x = 0xC0; x < (rom.size() - maxIdSize); x += 4)
    {
        for (const auto& [id, type] : save_types)
        {
            if (std::memcmp(id.data(), &rom[x], id.size()) == 0)
                return type;
        }
    }
    return Type::None;
}

u8 Save::read(u32 addr)
{
    return 0;
}

void Save::write(u32 addr, u8 byte)
{

}
