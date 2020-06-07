#include "flash.h"

#include <algorithm>

#include "base/macros.h"

Flash::Flash(const fs::path& file, uint size)
    : Save(file, size == 0x10'000 ? Type::Flash64 : Type::Flash128)
{
    data.resize(size, 0xFF);

    id = false;
    erase = false;
    command = 0;
    bank = &data[0];
}

u8 Flash::read(u32 addr)
{
    if (id)
    {
        switch (addr)
        {
        case 0: return data.size() == 0x20000 ? 0xC2 : 0xBF;
        case 1: return data.size() == 0x20000 ? 0x09 : 0xD4;
        }
    }
    return bank[addr];
}

void Flash::write(u32 addr, u8 byte)
{
    switch (command)
    {
    case CMD_WRITE_BYTE:
        bank[addr] = byte;
        command = 0;
        return;

    case CMD_SWITCH_BANK:
        if (data.size() == 0x20000)
            bank = &data[(byte & 0x1) * 0x10000];
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
        command <<= 8;
        command |= byte;

        switch (command)
        {
        case CMD_ERASE:
            erase = true;
            break;

        case CMD_ERASE_CHIP:
            if (erase)
            {
                std::fill(data.begin(), data.end(), 0xFF);
                erase = false;
            }
            break;

        case CMD_CHIP_ID_ENTER:
            id = true;
            break;

        case CMD_CHIP_ID_EXIT:
            id = false;
            break;
        }
        break;

    default:
        if ((addr & 0xFFF) == 0 && byte == 0x30)
        {
            command <<= 8;
            command |= byte;

            if (erase && command == CMD_ERASE_SECTOR)
            {
                u8* sector = bank + (addr & 0xF000);
                std::fill_n(sector, 0x1000, 0xFF);
                erase = false;
            }
        }
        break;
    }
}
