#include "eeprom.h"

#include "common/format.h"
#include "file.h"

EEPROM::EEPROM(const std::string& file, std::size_t rom_size)
    : Save(file, Save::Type::EEPROM)
{

    reset();
    state = SF_RECEIVE;

    if (rom_size == 0x2000000)
    {
        bus = 14;
        data.resize(0x2000, 0);
    }
    else
    {
        bus = 6;
        data.resize(0x0200, 0);
    }

    if (File::exists(file))
        File::read(file, data);
}

EEPROM::~EEPROM()
{
    // Todo: write data to file
}

u8 EEPROM::readByte(u32)
{
    switch (state)
    {
    case SF_READ_NIBBLE:
        if (++transmitted == 4)
        {
            fmt::printf("EEPROM read nibble: skipped\n");
            state = SF_READ;
            reset();
        }
        break;

    case SF_READ:
    {
        int off = transmitted / 8;
        int bit = transmitted % 8;

        if (++transmitted == 64)
        {
            fmt::printf("EEPROM read: finished\n");
            state = SF_RECEIVE;
            reset();
        }
        return (data[addr + off] >> (7 - bit)) & 0x1;
    }

    default:
        fmt::printf("EEPROM: unexpected read\n");
        return 0;
    }
    return 1;
}

void EEPROM::writeByte(u32, u8 byte)
{
    if (state == SF_READ || state == SF_READ_NIBBLE)
    {
        fmt::printf("EEPROM unexpected write\n");
        return;
    }

    byte &= 0x1;

    transmitted++;
    buffer = (buffer << 1) | byte;

    switch (state)
    {
    case SF_RECEIVE:
        if (transmitted == 2)
        {
            if (buffer == 0b10)
            {
                state = SF_WRITE_ADDRESS;
                fmt::printf("EEPROM receive: start write address\n");
            }
            else
            {
                state = SF_READ_ADDRESS;
                fmt::printf("EEPROM receive: start read address\n");
            }
            addr = 0;
            reset();
        }
        break;

    case SF_READ_ADDRESS:
        if (transmitted == bus)
        {
            addr = buffer * 8;
            fmt::printf("EEPROM read address: %X\n", addr);
        }
        else if (transmitted > bus)
        {
            if (byte & 0x1)
                fmt::printf("EEPROM read address: unexpected end\n");

            fmt::printf("EEPROM read address: end\n");

            state = SF_READ_NIBBLE;
            reset();
        }
        break;

    case SF_WRITE_ADDRESS:
        if (transmitted == bus)
        {
            addr = buffer * 8;
            fmt::printf("EEPRON write address: %X\n", addr);

            state = SF_WRITE;
            reset();
        }
        break;

    case SF_WRITE:
    {
        if (transmitted <= 64)
        {
            int off = (transmitted - 1) / 8;
            int bit = (transmitted - 1) % 8;

            data[addr + off] &= ~(1 << (7 - bit));
            data[addr + off] |= byte << (7 - bit);
            if (transmitted == 64)
                fmt::printf("EEPROM write: finished\n");
        }
        else if (transmitted > 64)
        {
            if (byte & 0x1)
                fmt::printf("EEPROM write: unexpected end\n");

            state = SF_RECEIVE;
            reset();
            fmt::printf("EEPROM write: end received\n");
        }
        break;
    }
    }
}

void EEPROM::reset()
{
    buffer = 0;
    transmitted = 0;
}
