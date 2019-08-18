#include "flash.h"

#include <algorithm>

#include "file.h"

Flash::Flash(const std::string& file, Save::Type type)
    : Save(file, type)
{
    switch (type)
    {
    case Save::Type::FLASH64:
        data.resize(0x10000, 0xFF);
        break;

    case Save::Type::FLASH128:
        data.resize(0x20000, 0xFF);
        break;
    }

    if (File::exists(file))
        File::read(file, data);

    id = false;
    erase = false;
    command = 0;
    bank = &data[0];
}

Flash::~Flash()
{
    File::write(file, data);
}

u8 Flash::readByte(u32 addr)
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

void Flash::writeByte(u32 addr, u8 byte)
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
