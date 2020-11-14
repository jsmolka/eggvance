#pragma once

#include "save.h"

class Flash : public Save
{
public:
    static constexpr uint kSize512  = 0x10'000;
    static constexpr uint kSize1024 = 0x20'000;

    Flash(uint size);

    void reset() final;

    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

    const uint size;

protected:
    bool isValid(uint size) const final;

private:
    enum Command
    {
        kCommandErase       = 0xAA5580,
        kCommandEraseChip   = 0xAA5510,
        kCommandEraseSector = 0xAA5530,
        kCommandWriteByte   = 0xAA55A0,
        kCommandSwitchBank  = 0xAA55B0,
        kCommandChipEnter   = 0xAA5590,
        kCommandChipExit    = 0xAA55F0
    };

    enum Chip
    {
        kChipMacronix512  = 0x1CC2,
        kChipMacronix1024 = 0x09C2
    };

    uint command = 0;
    bool chip    = false;
    bool erase   = false;
    u8*  bank    = nullptr;
};
