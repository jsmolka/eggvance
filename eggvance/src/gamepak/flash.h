#pragma once

#include <algorithm>
#include <shell/macros.h>
#include <shell/operators.h>

#include "save.h"
#include "base/bit.h"

template<Save::Type Kind>
class Flash final : public Save
{
public:
    static_assert(Kind == Type::Flash512 || Kind == Type::Flash1024);

    Flash()
        : Save(Kind)
    {
        data.resize(size(), 0xFF);

        reset();
    }

    constexpr auto size() const
    {
        return (Kind == Type::Flash512 ? 64 : 128) * 1024;
    }

    void reset() final
    {
        command = 0;
        chip    = false;
        erase   = false;
        bank    = data.data();
    }

    u8 read(u32 addr) final
    {
        if (chip && addr <= 1)
        {
            uint macronix = Kind == Type::Flash512
                ? 0x1CC2
                : 0x09C2;

            return bit::byte(macronix, addr);
        }
        return bank[addr];
    }

    void write(u32 addr, u8 byte) final
    {
        switch (Command(command))
        {
        case Command::WriteByte:
            changed = true;
            bank[addr] = byte;
            command = 0;
            return;

        case Command::SwitchBank:
            if (Kind == Type::Flash1024)
                bank = data.data() + (size() / 2) * (byte & 0x1);
            command = 0;
            return;
        }

        switch (addr)
        {
        case 0x5555:
            if (byte == 0xAA)
                command = 0;
            [[fallthrough]];

        case 0x2AAA:
            command = (command << 8) | byte;

            switch (Command(command))
            {
            case Command::Erase:
                erase = true;
                break;

            case Command::EraseChip:
                if (erase)
                {
                    changed = true;
                    std::fill(data.begin(), data.end(), 0xFF);
                    erase = false;
                }
                break;

            case Command::ChipEnter:
            case Command::ChipExit:
                chip = command == Command::ChipEnter;
                break;
            }
            break;

        default:
            if ((addr & 0xFFF) == 0 && byte == 0x30)
            {
                command = (command << 8) | byte;

                if (erase && command == Command::EraseSector)
                {
                    changed = true;
                    u8* sector = bank + (addr & 0xF000);
                    std::fill_n(sector, 0x1000, 0xFF);
                    erase = false;
                }
            }
            break;
        }
    }

protected:
    bool valid(uint size) const final
    {
        return this->size() == size;
    }

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

    uint command = 0;
    bool chip    = false;
    bool erase   = false;
    u8*  bank    = nullptr;
};

using Flash512  = Flash<Save::Type::Flash512>;
using Flash1024 = Flash<Save::Type::Flash1024>;
