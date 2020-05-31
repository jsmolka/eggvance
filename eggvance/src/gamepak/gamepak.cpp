#include "gamepak.h"

#include <stdexcept>

#include "base/config.h"
#include "gamepak/eeprom.h"
#include "gamepak/flash.h"
#include "gamepak/sram.h"

std::size_t GamePak::size() const
{
    return rom.size();
}

u8 GamePak::readByte(u32 addr) const
{
    if (addr < rom.size())
        return *reinterpret_cast<const u8*>(&rom[addr]);
    else
        return readUnused(addr);
}

u16 GamePak::readHalf(u32 addr) const
{
    if (addr < rom.size())
        return *reinterpret_cast<const u16*>(&rom[addr]);
    else
        return readUnused(addr);
}

u32 GamePak::readWord(u32 addr) const
{
    if (addr < rom.size())
        return *reinterpret_cast<const u32*>(&rom[addr]);
    else
        return readUnused(addr);
}

void GamePak::load(const fs::path& rom_file, const fs::path& save_file)
{
    if (!fs::read(rom_file, rom))
        throw std::runtime_error("Cannot read file");

    header = Header(rom);

    initSave(save_file, Save::parse(rom));
}

void GamePak::load(const fs::path& rom_file)
{
    auto save_file = fs::path(rom_file).replace_extension("sav");
    if (!config.save_path.empty())
        save_file = config.save_path / save_file.filename();

    load(rom_file, save_file);
}

void GamePak::loadSave(const fs::path& save_file)
{
    initSave(save_file, save->type);
    fs::read(save_file, save->data);
}

u32 GamePak::readUnused(u32 addr)
{
    addr = (addr & ~0x3) >> 1;
    return (addr & 0xFFFF) | ((addr + 1) & 0xFFFF) << 16;
}

void GamePak::initSave(const fs::path& file, Save::Type type)
{
    save = nullptr;

    switch (type)
    {
    case Save::Type::Sram:
        save = std::make_unique<Sram>(file);
        break;

    case Save::Type::Eeprom:
        save = std::make_unique<Eeprom>(file);
        break;

    case Save::Type::Flash64:
        save = std::make_unique<Flash64>(file);
        break;

    case Save::Type::Flash128:
        save = std::make_unique<Flash128>(file);
        break;

    default:
        save = std::make_unique<Save>();
        break;
    }
}
