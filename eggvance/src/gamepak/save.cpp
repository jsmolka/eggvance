#include "save.h"

#include <algorithm>
#include <string_view>
#include <tuple>

#include "frontend/videocontext.h"

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
        if (fs::write(file, data) != fs::Status::Ok)
            video_ctx.showMessageBox("Warning", "Cannot write save: {}", file);
    }
}

Save::Type Save::parse(const Rom& rom)
{
    static constexpr std::tuple<std::string_view, Save::Type> kSignatures[6] =
    {
        { "SRAM_V",     Save::Type::Sram      },
        { "SRAM_F_V",   Save::Type::Sram      },
        { "EEPROM_V",   Save::Type::Eeprom    },
        { "FLASH_V",    Save::Type::Flash512  },
        { "FLASH512_V", Save::Type::Flash512  },
        { "FLASH1M_V",  Save::Type::Flash1024 }
    };

    for (std::size_t addr = sizeof(Rom::Header); addr < rom.size(); addr += 4)
    {
        for (const auto& [signature, type] : kSignatures)
        {
            if (addr + signature.size() < rom.size()
                    && std::equal(signature.begin(), signature.end(), rom.begin() + addr))
                return type;
        }
    }
    return Type::None;
}

bool Save::load(const fs::path& file)
{
    if (fs::is_regular_file(file))
    {
        std::size_t size = data.size();

        if (fs::read(file, data) != fs::Status::Ok)
        {
            video_ctx.showMessageBox("Warning", "Cannot read save: {}\nProgress will not be saved", file);
            resize(size);
            return false;
        }

        if (!isValidSize(data.size()))
        {
            video_ctx.showMessageBox("Warning", "Invalid save size: {} bytes\nProgress will not be saved", data.size());
            resize(size);
            return false;
        }
    }
    this->file = file;

    return true;
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

void Save::resize(std::size_t size)
{
    data.resize(size);

    std::fill(data.begin(), data.end(), 0xFF);
}

bool Save::isValidSize(uint size) const
{
    return size == 0;
}
