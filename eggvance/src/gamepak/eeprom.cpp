#include "eeprom.h"

inline constexpr auto kSize512Bytes   =      512;
inline constexpr auto kSize8Kilobytes = 8 * 1024;

Eeprom::Eeprom()
    : Save(Type::Eeprom)
{

}

void Eeprom::initBus6()
{
    resize(kSize512Bytes);
}

void Eeprom::initBus14()
{
    resize(kSize8Kilobytes);
}

bool Eeprom::isInitialized() const
{
    return !data.empty();
}

void Eeprom::reset()
{
    address = 0;
    setState(State::Receive);
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
            if (address < data.size())
                buffer = *reinterpret_cast<u64*>(data.data() + address);
            else
                buffer = std::numeric_limits<u64>::max();
            buffer.size = 64;
        }
        break;

    case State::Read:
        bit = buffer.popl();
        if (buffer.size == 0)
            setState(State::Receive);
        break;
    }
    return bit;
}

void Eeprom::write(u32 addr, u8 byte)
{
    if (state == State::Read || state == State::ReadUnused)
        return;

    buffer.pushr(byte);

    switch (state)
    {
    case State::Receive:
        if (buffer.size == 2)
        {
            static constexpr State kStates[4] =
            {
                State::Receive,
                State::Receive,
                State::WriteSetAddress,
                State::ReadSetAddress
            };

            address = 0;
            setState(kStates[buffer]);
        }
        break;

    case State::ReadSetAddress:
        if (buffer.size == bus())
        {
            address = buffer << 3;
            setState(State::ReadSetAddressEnd);
        }
        break;

    case State::ReadSetAddressEnd:
        setState(State::ReadUnused);
        break;

    case State::WriteSetAddress:
        if (buffer.size == bus())
        {
            address = buffer << 3;
            setState(State::Write);
        }
        break;

    case State::Write:
        if (buffer.size == 64)
        {
            if (address < data.size())
            {
                changed = true;
                *reinterpret_cast<u64*>(data.data() + address) = buffer;
            }
            setState(State::WriteEnd);
        }
        break;

    case State::WriteEnd:
        setState(State::Receive);
        break;
    }
}

bool Eeprom::isValidSize(uint size) const
{
    return size == kSize512Bytes
        || size == kSize8Kilobytes;
}

uint Eeprom::bus() const
{
    SHELL_ASSERT(data.size() == kSize512Bytes || data.size() == kSize8Kilobytes);

    return data.size() == kSize512Bytes ? 6 : 14;
}

void Eeprom::setState(State state)
{
    this->state = state;
    this->buffer.clear();
}
