#include "save.h"

#include <algorithm>
#include <string_view>
#include <utility>

#include "header.h"
#include "base/panic.h"

Save::Save()
    : type(Type::None)
{

}

Save::Save(Type type)
    : type(type)
{

}

Save::~Save()
{
    if (changed
        && !file.empty()
        && !data.empty()
        && type != Type::None)
    {
        if (!fs::write(file, data))
            panic("Cannot write save: {}", file);
    }
}

Save::Type Save::parse(const std::vector<u8>& rom)
{
    static constexpr std::pair<std::string_view, Save::Type> kSignatures[] =
    {
        { "SRAM_V"    , Save::Type::Sram      },
        { "SRAM_F_V"  , Save::Type::Sram      },
        { "EEPROM_V"  , Save::Type::Eeprom    },
        { "FLASH_V"   , Save::Type::Flash512  },
        { "FLASH512_V", Save::Type::Flash512  },
        { "FLASH1M_V" , Save::Type::Flash1024 }
    };

    for (uint x = Header::kSize; x < rom.size(); x += 4)
    {
        for (const auto& [signature, type] : kSignatures)
        {
            if (x + signature.size() >= rom.size())
                continue;

            if (std::equal(signature.begin(), signature.end(), &rom[x]))
                return type;
        }
    }
    return Type::None;
}

void Save::init(const fs::path& file)
{
    this->file = file;

    if (fs::is_regular_file(file))
    {
        if (!fs::read(file, data))
            panic("Cannot read save: {}", file);

        if (!isValid(data.size()))
            panic("Invalid save size: {}", data.size());
    }
}

void Save::reset()
{

}

u8 Save::read(u32 addr)
{
    return 0;
}

void Save::write(u32 addr, u8 byte)
{

}

bool Save::isValid(uint size) const
{
    return true;
}
