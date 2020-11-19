#pragma once

#include <memory>

#include "gpio.h"
#include "rom.h"
#include "save.h"

class GamePak
{
public:
    void load(fs::path gba, fs::path sav);

    bool isEepromAccess(u32 addr) const;

    template<typename Integral>
    Integral read(u32 addr) const
    {
        addr &= rom.mask - sizeof(Integral);

        if (sizeof(Integral) > 1 
                && addr <= Gpio::kRegControl 
                && addr >= Gpio::kRegData
                && gpio->type != Gpio::Type::None
                && gpio->isReadable())
            return gpio->read(addr);

        return rom.read<Integral>(addr);
    }

    template<typename Integral>
    void write(u32 addr, Integral value)
    {
        addr &= rom.mask - sizeof(Integral);

        if (sizeof(Integral) > 1
                && addr <= Gpio::kRegControl 
                && addr >= Gpio::kRegData
                && gpio->type != Gpio::Type::None)
            gpio->write(addr, value);
    }

    Rom rom;
    std::unique_ptr<Gpio> gpio;
    std::unique_ptr<Save> save;
};

inline GamePak gamepak;
