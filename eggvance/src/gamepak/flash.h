#pragma once

#include "gamepak/save.h"

template<uint size>
class Flash : public Save
{
    static_assert(size == 0x10'000 || size == 0x20'000);

public:
    explicit Flash(const fs::path& file);

    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

    static constexpr uint kSize = size;

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
        kChipMacronix64  = 0x1CC2,
        kChipMacronix128 = 0x09C2
    };

    bool chip = false;
    bool erase = false;
    uint command = 0;
    u8* bank = nullptr;
};

using Flash64  = Flash<0x10'000>;
using Flash128 = Flash<0x20'000>;
