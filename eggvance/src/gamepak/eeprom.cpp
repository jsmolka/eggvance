#include "eeprom.h"

#include <shell/utility.h>

#include "base/log.h"

Eeprom::Eeprom()
    : Save(Type::Eeprom)
{

}

void Eeprom::reset()
{
    shell::reconstruct(*this);
}

u8 Eeprom::read(u32 addr)
{
    uint bit = 1;
    switch (state)
    {
    case State::ReadUnused:
        if (++buffer.size == 4)
        {
            setState(State::Read);
            buffer.data = *reinterpret_cast<u64*>(data.data() + address);
            buffer.size = 64;
        }
        break;

    case State::Read:
        bit = buffer.popl();
        if (buffer.size == 0)
            setState(State::Receive);
        break;

    default:
        SHELL_LOG_WARN("Bad state {}", state);
        break;
    }
    return bit;
}

void Eeprom::write(u32 addr, u8 byte)
{
    if (state == State::Read || state == State::ReadUnused)
    {
        SHELL_LOG_WARN("Bad state {}", state);
        return;
    }

    buffer.pushr(byte);

    switch (state)
    {
    case State::Receive:
        if (buffer.size == 2)
        {
            if (buffer.data < 2)
                SHELL_LOG_WARN("Bad receive {}", buffer.data);

            static constexpr State kStates[4] = {
                State::Receive,
                State::Receive,
                State::WriteSetAddress,
                State::ReadSetAddress
            };

            address = 0;
            setState(kStates[buffer.data]);
        }
        break;

    case State::ReadSetAddress:
        if (buffer.size == bus())
        {
            address = buffer.data << 3;
            setState(State::ReadSetAddressEnd);
        }
        break;

    case State::ReadSetAddressEnd:
        setState(State::ReadUnused);
        break;

    case State::WriteSetAddress:
        if (buffer.size == bus())
        {
            address = buffer.data << 3;
            setState(State::Write);
        }
        break;

    case State::Write:
        if (buffer.size == 64)
        {
            changed = true;
            *reinterpret_cast<u64*>(data.data() + address) = buffer.data;
            setState(State::WriteEnd);
        }
        break;

    case State::WriteEnd:
        setState(State::Receive);
        break;
    }
}

bool Eeprom::isValid(uint size) const
{
    return size == kSize4
        || size == kSize64;
}

uint Eeprom::bus() const
{
    return data.size() == kSize4 ? 6 : 14;
}

void Eeprom::setState(State state)
{
    this->state = state;
    this->buffer.clear();
}
