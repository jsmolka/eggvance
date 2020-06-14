#include "flash.h"

#include <algorithm>

template<uint size>
Flash<size>::Flash(const fs::path& file)
    : Save(file, size == 0x10'000 ? Type::Flash64 : Type::Flash128)
{
    data.resize(size, 0xFF);

    bank = data.data();
}

template<uint size>
u8 Flash<size>::read(u32 addr)
{
    constexpr uint macronix = size == 0x10'000
        ? kChipMacronix64
        : kChipMacronix128;

    if (chip && addr < 2)
        return (macronix >> (8 * addr)) & 0xFF;

    return bank[addr];
}

template<uint size>
void Flash<size>::write(u32 addr, u8 byte)
{
    switch (command)
    {
    case kCommandWriteByte:
        bank[addr] = byte;
        command = 0;
        return;

    case kCommandSwitchBank:
        if (size == 0x20'000)
            bank = &data[0x10'000 * (byte & 0x1)];
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

        switch (command)
        {
        case kCommandErase:
            erase = true;
            break;

        case kCommandEraseChip:
            if (erase)
            {
                std::fill_n(data.begin(), size, 0xFF);
                erase = false;
            }
            break;

        case kCommandChipEnter:
        case kCommandChipExit:
            chip = command == kCommandChipEnter;
            break;
        }
        break;

    default:
        if ((addr & 0xFFF) == 0 && byte == 0x30)
        {
            command = (command << 8) | byte;

            if (erase && command == kCommandEraseSector)
            {
                u8* sector = bank + (addr & 0xF000);
                std::fill_n(sector, 0x1000, 0xFF);
                erase = false;
            }
        }
        break;
    }
}

template class Flash<0x10'000>;
template class Flash<0x20'000>;
