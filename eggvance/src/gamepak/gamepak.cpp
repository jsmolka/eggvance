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

    if (const auto overwrite = Overwrite::find(header.code))
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
    // Todo: check necessary things for save load

    initSave(save_file, save->type);
}

u32 GamePak::readUnused(u32 addr)
{
    addr = (addr & ~0x3) >> 1;
    return (addr & 0xFFFF) | ((addr + 1) & 0xFFFF) << 16;
}

void GamePak::initGpio(Gpio::Type type)
{
    gpio = std::invoke([&]() -> std::unique_ptr<Gpio>
    {
        if (type == Gpio::Type::Rtc)
            return std::make_unique<Rtc>();
        
        return std::make_unique<Gpio>();
    });
}

void GamePak::initSave(const fs::path& file, Save::Type type)
{
    save = std::invoke([&]() -> std::unique_ptr<Save>
    {
        switch (type)
        {
        case Save::Type::Sram:      return std::make_unique<Sram>();
        case Save::Type::Eeprom:    return std::make_unique<Eeprom>();
        case Save::Type::Flash512:  return std::make_unique<Flash>(Flash::kSize512);
        case Save::Type::Flash1024: return std::make_unique<Flash>(Flash::kSize1024);
        }
        return std::make_unique<Save>();
    });

    if (!save->init(file))
    {
        if (const auto overwrite = Overwrite::find(header.code))
            initSave(fs::path(), overwrite->save_type);
        else
            initSave(fs::path(), Save::parse(rom));
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
