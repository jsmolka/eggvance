#include "eeprom.h"

Eeprom::Eeprom(const fs::path& file)
    : Save(file, Save::Type::Eeprom)
{
    state = State::Receive;
}

u8 Eeprom::read(u32 addr)
{
    switch (state)
    {
    case State::ReadUnused:
        if (++count == 4)
            setState(State::Read);
        break;

    case State::Read:
        {
            uint index  = count % 8;
            uint offset = count / 8;

            if (++count == 64)
                setState(State::Receive);

            return (data[address + offset] >> (7 - index)) & 0x1;
        }
    }
    return 1;
}

void Eeprom::write(u32 addr, u8 byte)
{
    if (state == State::Read || state == State::ReadUnused)
        return;

    count++;
    buffer = (buffer << 1) | (byte & 0x1);

    switch (state)
    {
    case State::Receive:
        if (count == 2)
        {
            static constexpr State states[4] = {
                State::Receive,
                State::Receive,
                State::WriteSetAddress,
                State::ReadSetAddress
            };

            address = 0;
            setState(states[buffer]);
        }
        break;

    case State::ReadSetAddress:
        if (count == (data.size() == 0x2000 ? 14 : 6))
        {
            address = buffer << 3;
            setState(State::ReadSetAddressEnd);
        }
        break;

    case State::ReadSetAddressEnd:
        setState(State::ReadUnused);
        break;

    case State::WriteSetAddress:
        if (count == (data.size() == 0x2000 ? 14 : 6))
        {
            address = buffer << 3;
            setState(State::Write);
        }
        break;

    case State::Write:
        if (count % 8 == 0)
        {
            data[address++] = buffer;
            buffer = 0;

            if (count == 64)
                setState(State::WriteEnd);
        }
        break;

    case State::WriteEnd:
        setState(State::Receive);
        break;
    }
}

void Eeprom::setState(State state)
{
    this->state = state;

    count = 0;
    buffer = 0;
}
