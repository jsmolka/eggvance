#include "gamepak.h"

#include <stdexcept>

#include "base/config.h"
#include "gamepak/eeprom.h"
#include "gamepak/flash.h"
#include "gamepak/rtc.h"
#include "gamepak/sram.h"

std::size_t GamePak::size() const
{
    return rom.size();
}

u8 GamePak::readByte(u32 addr) const
{
    return read<u8>(addr);
}

u16 GamePak::readHalf(u32 addr) const
{
    return read<u16>(addr);
}

u32 GamePak::readWord(u32 addr) const
{
    return read<u32>(addr);
}

void GamePak::load(const fs::path& rom_file, const fs::path& save_file)
{
    if (!fs::read(rom_file, rom))
        throw std::runtime_error("Cannot read file");

    header = Header(rom);

    Save::Type save_type = config.save_type;
    Gpio::Type gpio_type = config.gpio_type;

    if (const auto overwrite = findOverwrite(header.code))
    {
        save_type = overwrite->save_type;
        gpio_type = overwrite->gpio_type;
    }
    else
    {
        if (save_type == Save::Type::None) 
            save_type = Save::parse(rom);
    }

    initGpio(gpio_type);
    initSave(save_file, save_type);
}

void GamePak::load(const fs::path& rom_file)
{
    auto save_file = rom_file;
    save_file.replace_extension("sav");

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

void GamePak::initGpio(Gpio::Type type)
{
    gpio = nullptr;

    switch (type)
    {
    case Gpio::Type::Rtc:
        gpio = std::make_unique<Rtc>();
        break;

    default:
        gpio = std::make_unique<Gpio>();
        break;
    }
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

template<typename T>
T GamePak::read(u32 addr) const
{
    addr &= 0x200'0000 - sizeof(T);

    if (addr < rom.size())
        return *reinterpret_cast<const T*>(&rom[addr]);
    else
        return readUnused(addr);
}
