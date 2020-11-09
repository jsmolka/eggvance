#include "gamepak.h"

#include <shell/traits.h>

#include "eeprom.h"
#include "flash.h"
#include "rtc.h"
#include "sram.h"
#include "base/config.h"
#include "base/log.h"
#include "base/panic.h"

uint GamePak::size() const
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

void GamePak::writeByte(u32 addr, u8 byte)
{
    write<u8>(addr, byte);
}

void GamePak::writeHalf(u32 addr, u16 half)
{
    write<u16>(addr, half);
}

void GamePak::writeWord(u32 addr, u32 word)
{
    write<u32>(addr, word);
}

bool GamePak::isEepromAccess(u32 addr) const
{
    return save->type == Save::Type::Eeprom && size() <= 0x100'0000
        ? addr >= 0xD00'0000 && addr < 0xE00'0000
        : addr >= 0xDFF'FF00 && addr < 0xE00'0000;
}

void GamePak::loadRom(const fs::path& file, bool save)
{
    if (!fs::read(file, rom))
        panic("Cannot read ROM: {}", file);

    header = Header(rom);

    const auto overwrite = Overwrite::find(header.code);

    initGpio(overwrite ? overwrite->gpio : config.gpio);

    if (save)
    {
        fs::path save(file);
        save.replace_extension("sav");

        if (!config.save_path.empty())
            save = config.save_path / save.filename();

        loadSave(save);
    }
}

void GamePak::loadSave(const fs::path& file)
{
    if (rom.empty())
        return;

    Save::Type type = config.save;

    if (const auto overwrite = Overwrite::find(header.code))
        type = overwrite->save;

    initSave(file, type == Save::Type::None ? Save::parse(rom) : type);
}

template<typename Integral>
Integral GamePak::read(u32 addr) const
{
    static_assert(shell::is_any_of_v<Integral, u8, u16, u32>);

    if (isEepromAccess(addr))
        return 1;

    addr &= 0x200'0000 - sizeof(Integral);

    if (gpio->isAccess(addr))
        return gpio->read(addr);

    if (addr < rom.size())
        return *reinterpret_cast<const Integral*>(rom.data() + addr);

    SHELL_LOG_WARN("Invalid address {:08X}", addr);

    addr = (addr & ~0x3) >> 1;
    return (addr & 0xFFFF) | ((addr + 1) & 0xFFFF) << 16;
}

template<typename Integral>
void GamePak::write(u32 addr, Integral value)
{
    static_assert(shell::is_any_of_v<Integral, u8, u16, u32>);

    addr &= 0x200'0000 - sizeof(Integral);

    if (gpio->isAccess(addr))
        gpio->write(addr, value);
}

void GamePak::initGpio(Gpio::Type type)
{
    gpio = std::invoke([&]() -> std::unique_ptr<Gpio>
    {
        switch (type)
        {
        case Gpio::Type::Rtc: return std::make_unique<Rtc>();
        }
        return std::make_unique<Gpio>();
    });

    gpio->reset();
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

    save->init(file);
}
