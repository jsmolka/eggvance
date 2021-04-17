#include "gamepak.h"

#include "eeprom.h"
#include "flash.h"
#include "overwrites.h"
#include "rtc.h"
#include "sram.h"
#include "base/config.h"

void GamePak::load(fs::path gba, fs::path sav)
{
    if (!gba.empty())
    {
        rom.init(gba);

        if (sav.empty())
        {
            sav = gba;
            sav.replace_extension("sav");

            if (!config.save_path.empty())
                sav = config.save_path / sav.filename();
        }
    }

    Save::Type save_type = config.save_type;
    Gpio::Type gpio_type = config.gpio_type;

    if (const auto overwrite = Overwrite::find(rom.code))
    {
        save_type = overwrite->save_type;
        gpio_type = overwrite->gpio_type;
        rom.mask  = overwrite->mirror ? rom.size() : Rom::kMaxSize;
    }

    switch (gpio_type)
    {
    case Gpio::Type::Detect: [[fallthrough]];
    case Gpio::Type::None:   gpio = std::make_shared<Gpio>(); break;
    case Gpio::Type::Rtc:    gpio = std::make_shared<Rtc>(); break;
    }

    if (!sav.empty() && rom.size())
    {
        if (save_type == Save::Type::Detect)
            save_type =  Save::parse(rom);

        switch (save_type)
        {
        case Save::Type::Detect:    [[fallthrough]];
        case Save::Type::None:      save = std::make_shared<Save>(); break;
        case Save::Type::Sram:      save = std::make_shared<Sram>(); break;
        case Save::Type::Eeprom:    save = std::make_shared<Eeprom>(); break;
        case Save::Type::Flash512:  save = std::make_shared<Flash512>(); break;
        case Save::Type::Flash1024: save = std::make_shared<Flash1024>(); break;
        }
        save->init(sav);
    }
}

bool GamePak::isEepromAccess(u32 addr) const
{
    return rom.size() == Rom::kMaxSize
            ? addr >= 0xDFF'FF00 && addr < 0xE00'0000
            : addr >= 0xD00'0000 && addr < 0xE00'0000
        && save->type == Save::Type::Eeprom;
}

u8 GamePak::readSave(u32 addr)
{
    switch (save->type)
    {
    case Save::Type::Sram:
        addr &= 0x7FFF;
        return save->read(addr);

    case Save::Type::Flash512:
    case Save::Type::Flash1024:
        addr &= 0xFFFF;
        return save->read(addr);

    default:
        return 0xFF;
    }
}

void GamePak::writeSave(u32 addr, u8 byte)
{
    switch (save->type)
    {
    case Save::Type::Sram:
        addr &= 0x7FFF;
        save->write(addr, byte);
        break;

    case Save::Type::Flash512:
    case Save::Type::Flash1024:
        addr &= 0xFFFF;
        save->write(addr, byte);
        break;
    }
}
