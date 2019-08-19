#include "eeprom.h"

EEPROM::EEPROM(const std::string& file)
    : Save(file, Save::Type::EEPROM)
{
    reset();
    state = SF_RECEIVE;
}

u8 EEPROM::readByte(u32)
{
    switch (state)
    {
    case SF_READ_NIBBLE:
        if (++transmitted == 4)
        {
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
            state = SF_RECEIVE;
            reset();
        }
        return (data[addr + off] >> (7 - bit)) & 0x1;
    }
    }
    return 1;
}

void EEPROM::writeByte(u32, u8 byte)
{
    if (state == SF_READ || state == SF_READ_NIBBLE)
        return;

    byte &= 0x1;

    transmitted++;
    buffer = (buffer << 1) | byte;

    switch (state)
    {
    case SF_RECEIVE:
        if (transmitted == 2)
        {
            switch (buffer)
            {
            case 0b10:
                state = SF_WRITE_ADDRESS;
                break;

            case 0b11:
                state = SF_READ_ADDRESS;
                break;
            }
            addr = 0;
            reset();
        }
        break;

    case SF_READ_ADDRESS:
        if (transmitted == bus())
        {
            addr = buffer << 3;
        }
        else if (transmitted > bus())
        {
            state = SF_READ_NIBBLE;
            reset();
        }
        break;

    case SF_WRITE_ADDRESS:
        if (transmitted == bus())
        {
            addr = buffer << 3;
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
        }
        else if (transmitted > 64)
        {
            state = SF_RECEIVE;
            reset();
        }
        break;
    }
    }
}

int EEPROM::bus() const
{
    return data.size() == 0x2000 ? 14 : 6;
}

void EEPROM::reset()
{
    buffer = 0;
    transmitted = 0;
}
