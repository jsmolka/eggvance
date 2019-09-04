#include "eeprom.h"

#include "common/fmt.h"

EEPROM::EEPROM(const std::string& file)
    : Save(file, Save::Type::EEPROM)
{
    resetBuffer();
    state = STATE_RECEIVE;
}

u8 EEPROM::readByte(u32)
{
    switch (state)
    {
    case STATE_READ_NIBBLE:
        if (++transmitted == 4)
        {
            state = STATE_READ;
            resetBuffer();
        }
        break;

    case STATE_READ:
    {
        int off = transmitted / 8;
        int bit = transmitted % 8;

        if (++transmitted == 64)
        {
            state = STATE_RECEIVE;
            resetBuffer();
        }
        return (data[addr + off] >> (7 - bit)) & 0x1;
    }

    default:
        fmt::printf("EEPROM: Reading in invalid state %d\n", static_cast<int>(state));
        break;
    }
    return 1;
}

void EEPROM::writeByte(u32, u8 byte)
{
    if (state == STATE_READ || state == STATE_READ_NIBBLE)
    {
        fmt::printf("EEPROM: Writing in read state %d\n", static_cast<int>(state));
        return;
    }

    byte &= 0x1;

    transmitted++;
    buffer = (buffer << 1) | byte;

    switch (state)
    {
    case STATE_RECEIVE:
        if (transmitted == 2)
        {
            switch (buffer)
            {
            case 0b10:
                state = STATE_WRITE_ADDRESS;
                break;

            case 0b11:
                state = STATE_READ_ADDRESS;
                break;

            default:
                fmt::printf("EEPROM: Received invalid state %d\n", static_cast<int>(state));
                break;
            }
            addr = 0;
            resetBuffer();
        }
        break;

    case STATE_READ_ADDRESS:
        if (transmitted == bus())
        {
            addr = buffer << 3;
        }
        else if (transmitted > bus())
        {
            state = STATE_READ_NIBBLE;
            resetBuffer();
        }
        break;

    case STATE_WRITE_ADDRESS:
        if (transmitted == bus())
        {
            addr = buffer << 3;
            state = STATE_WRITE;
            resetBuffer();
        }
        break;

    case STATE_WRITE:
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
            state = STATE_RECEIVE;
            resetBuffer();
        }
        break;
    }
    }
}

int EEPROM::bus() const
{
    return data.size() == 0x2000 ? 14 : 6;
}

void EEPROM::resetBuffer()
{
    buffer = 0;
    transmitted = 0;
}
