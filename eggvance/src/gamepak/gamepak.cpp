#include "gamepak.h"

#include "eeprom.h"
#include "flash.h"
#include "overwrites.h"
#include "rtc.h"
#include "sram.h"
#include "base/config.h"
#include "base/panic.h"

void GamePak::load(fs::path gba, fs::path sav)
{
    if (gba.extension() == ".gba")
    {
        rom = std::move(Rom(gba));

        if (sav.empty())
        {
            sav = gba.replace_extension("sav");
            if (!config.save_path.empty())
                sav = config.save_path / sav.filename();
        }
    }

    Gpio::Type gpio_type = config.gpio;
    Save::Type save_type = config.save;

    if (const auto overwrite = Overwrite::find(rom.code))
    {
        gpio_type = overwrite->gpio;
        save_type = overwrite->save;
        rom.mask  = overwrite->mirror ? rom.size : Rom::kMaxSize;
    }

    gpio = gpio_type == Gpio::Type::None
        ? std::make_unique<Gpio>()
        : std::make_unique<Rtc>();

    if (sav.extension() == ".sav")
    {
        if (save_type == Save::Type::None)
            save_type =  Save::parse(rom);

        switch (save_type)
        {
        case Save::Type::Sram:      save = std::make_unique<Sram>(); break;
        case Save::Type::Eeprom:    save = std::make_unique<Eeprom>(); break;
        case Save::Type::Flash512:  save = std::make_unique<Flash>(Flash::kSize512); break;
        case Save::Type::Flash1024: save = std::make_unique<Flash>(Flash::kSize1024); break;
        default:                    save = std::make_unique<Save>(); break;
        }
        save->init(sav);
    }
}

bool GamePak::isEepromAccess(u32 addr) const
{
    return save->type == Save::Type::Eeprom && rom.size < Rom::kMaxSize
        ? addr >= 0xD00'0000 && addr < 0xE00'0000
        : addr >= 0xDFF'FF00 && addr < 0xE00'0000;
}
