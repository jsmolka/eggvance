#include "gamepak.h"

#include "eeprom.h"
#include "flash.h"
#include "rtc.h"
#include "sram.h"
#include "base/config.h"
#include "base/log.h"
#include "base/panic.h"

constexpr uint kMaxRomSize = 0x200'0000;

uint GamePak::size() const
{
    return rom.size();
}

bool GamePak::isEepromAccess(u32 addr) const
{
    return save->type == Save::Type::Eeprom && size() <= 0x100'0000
        ? addr >= 0xD00'0000 && addr < 0xE00'0000
        : addr >= 0xDFF'FF00 && addr < 0xE00'0000;
}

u8  GamePak::readByte(u32 addr) const { return read<u8 >(addr); }
u16 GamePak::readHalf(u32 addr) const { return read<u16>(addr); }
u32 GamePak::readWord(u32 addr) const { return read<u32>(addr); }

void GamePak::writeByte(u32 addr, u8  byte) { write(addr, byte); }
void GamePak::writeHalf(u32 addr, u16 half) { write(addr, half); }
void GamePak::writeWord(u32 addr, u32 word) { write(addr, word); }

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
    if (rom.empty()) return;

    Save::Type type = config.save;

    if (const auto overwrite = Overwrite::find(header.code))
        type = overwrite->save;

    initSave(file, type == Save::Type::None ? Save::parse(rom) : type);
}

template<typename Integral>
Integral GamePak::read(u32 addr) const
{
    addr &= kMaxRomSize - sizeof(Integral);

    if (sizeof(Integral) > 1 
            && addr <= Gpio::kAddrControl 
            && addr >= Gpio::kAddrData
            && gpio->type != Gpio::Type::None
            && gpio->isReadable())
        return gpio->read(addr);

    if (addr < rom.size())
        return *reinterpret_cast<const Integral*>(rom.data() + addr);

    SHELL_LOG_WARN("Bad read {:08X}", addr);

    addr = (addr & ~0x3) >> 1;
    return (addr & 0xFFFF) | ((addr + 1) & 0xFFFF) << 16;
}

template<typename Integral>
void GamePak::write(u32 addr, Integral value)
{
    addr &= kMaxRomSize - sizeof(Integral);

    if (sizeof(Integral) > 1
            && addr <= Gpio::kAddrControl 
            && addr >= Gpio::kAddrData
            && gpio->type != Gpio::Type::None)
        gpio->write(addr, value);
    else
        SHELL_LOG_WARN("Bad write {:08X}", addr);
}

void GamePak::initGpio(Gpio::Type type)
{
    gpio = type == Gpio::Type::Rtc
        ? std::make_unique<Rtc>()
        : std::make_unique<Gpio>();
}

void GamePak::initSave(const fs::path& file, Save::Type type)
{
    switch (type)
    {
    case Save::Type::Sram:      save = std::make_unique<Sram>(); break;
    case Save::Type::Eeprom:    save = std::make_unique<Eeprom>(); break;
    case Save::Type::Flash512:  save = std::make_unique<Flash>(Flash::kSize512); break;
    case Save::Type::Flash1024: save = std::make_unique<Flash>(Flash::kSize1024); break;
    default:                    save = std::make_unique<Save>(); break;
    }
    save->init(file);
}
