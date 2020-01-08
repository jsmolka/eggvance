#include "eeprom.h"

EEPROM::EEPROM(const Path& file)
    : Backup(file, Backup::Type::EEPROM)
{
    resetBuffer();
    state = State::Receive;
}

u8 EEPROM::readByte(u32 addr)
{
    switch (state)
    {
    case State::ReadNibble:
        if (++transmitted == 4)
        {
            state = State::Read;
            resetBuffer();
        }
        break;

    case State::Read:
        {
            int off = transmitted / 8;
            int bit = transmitted % 8;

            if (++transmitted == 64)
            {
                state = State::Receive;
                resetBuffer();
            }
            return (data[address + off] >> (7 - bit)) & 0x1;
        }
    }
    return 1;
}

void EEPROM::writeByte(u32, u8 byte)
{
    if (state == State::Read || state == State::ReadNibble)
        return;

    byte &= 0x1;
    transmitted++;

    buffer <<= 1;
    buffer |= byte;

    switch (state)
    {
    case State::Receive:
        if (transmitted == 2)
        {
            switch (buffer)
            {
            case 0b10:
                state = State::WriteAddress;
                break;

            case 0b11:
                state = State::ReadAddress;
                break;
            }
            address = 0;
            resetBuffer();
        }
        break;

    case State::ReadAddress:
        if (transmitted == bus())
        {
            address = buffer << 3;
        }
        else if (transmitted > bus())
        {
            state = State::ReadNibble;
            resetBuffer();
        }
        break;

    case State::WriteAddress:
        if (transmitted == bus())
        {
            address = buffer << 3;
            state = State::Write;
            resetBuffer();
        }
        break;

    case State::Write:
        {
            if (transmitted <= 64)
            {
                int off = (transmitted - 1) / 8;
                int bit = (transmitted - 1) % 8;

                data[address + off] &= ~(1 << (7 - bit));
                data[address + off] |= byte << (7 - bit);
            }
            else if (transmitted > 64)
            {
                state = State::Receive;
                resetBuffer();
            }
            break;
        }
    }
}

void EEPROM::resetBuffer()
{
    buffer = 0;
    transmitted = 0;
}

int EEPROM::bus() const
{
    return data.size() == 0x2000 ? 14 : 6;
}
