#pragma once

#include "save.h"

class Flash : public Save
{
public:
    static constexpr auto kSize512  =  64 * 1024;
    static constexpr auto kSize1024 = 128 * 1024;

    Flash(uint size);

    void reset() final;
    u8 read(u32 addr) final;
    void write(u32 addr, u8 byte) final;

    const uint size;

protected:
    bool valid(uint size) const final;

private:
    enum class Command
    {
        Erase       = 0xAA5580,
        EraseChip   = 0xAA5510,
        EraseSector = 0xAA5530,
        WriteByte   = 0xAA55A0,
        SwitchBank  = 0xAA55B0,
        ChipEnter   = 0xAA5590,
        ChipExit    = 0xAA55F0
    };

    enum class Chip
    {
        Macronix512  = 0x1CC2,
        Macronix1024 = 0x09C2
    };

    uint command = 0;
    bool chip    = false;
    bool erase   = false;
    u8*  bank    = nullptr;
};
