#include "save.h"

#include <cstring>
#include <string_view>
#include <utility>

#include "gamepak/header.h"

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
    if (!data.empty())
        fs::write(file, data);
}

Save::Type Save::parse(const std::vector<u8>& rom)
{
    static constexpr std::pair<std::string_view, Save::Type> identifiers[] =
    {
        { "SRAM_V"    , Save::Type::Sram     },
        { "SRAM_F_V"  , Save::Type::Sram     },
        { "EEPROM_V"  , Save::Type::Eeprom   },
        { "FLASH_V"   , Save::Type::Flash64  },
        { "FLASH512_V", Save::Type::Flash64  },
        { "FLASH1M_V" , Save::Type::Flash128 }
    };

    for (uint x = Header::size; x < rom.size(); x += 4)
    {
        for (const auto& [id, type] : identifiers)
        {
            if (x + id.size() < rom.size()
                    && std::memcmp(&rom[x], id.data(), id.size()) == 0)
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
