#include "flash.h"

#include <algorithm>

#include "base/bit.h"
#include "base/macros.h"

Flash::Flash(uint size)
    : Save(size == kSize512 ? Type::Flash512 : Type::Flash1024)
    , size(size)
{
    SHELL_ASSERT(size == kSize512 || size == kSize1024);

    data.resize(size, 0xFF);

    reset();
}

void Flash::reset()
{
    command = 0;
    chip    = false;
    erase   = false;
    bank    = data.data();
}

u8 Flash::read(u32 addr)
{
    if (chip && addr <= 1)
    {
        uint macronix = size == kSize512
            ? kChipMacronix512
            : kChipMacronix1024;

        return bit::byte(macronix, addr);
    }
    return bank[addr];
}

void Flash::write(u32 addr, u8 byte)
{
    switch (command)
    {
    case kCommandWriteByte:
        changed = true;
        bank[addr] = byte;
        command = 0;
        return;

    case kCommandSwitchBank:
        if (size == kSize1024)
            bank = &data[kSize512 * (byte & 0x1)];
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
                changed = true;
                std::fill(data.begin(), data.end(), 0xFF);
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
                changed = true;
                u8* sector = bank + (addr & 0xF000);
                std::fill_n(sector, 0x1000, 0xFF);
                erase = false;
            }
        }
        break;
    }
}

bool Flash::isValid(uint size) const
{
    return this->size == size;
}
