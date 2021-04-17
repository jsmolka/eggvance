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

        if (sizeof(Integral) > 1 && gpio->isAccess(addr) && gpio->isReadable())
            return gpio->read(addr);

        return rom.read<Integral>(addr);
    }

    template<typename Integral>
    void write(u32 addr, Integral value)
    {
        addr &= rom.mask - sizeof(Integral);

        if (sizeof(Integral) > 1 && gpio->isAccess(addr))
            gpio->write(addr, value);
    }

    u8 readSave(u32 addr);
    void writeSave(u32 addr, u8 byte);

    Rom rom;
    std::shared_ptr<Save> save;
    std::shared_ptr<Gpio> gpio;
};

inline GamePak gamepak;
