#include "io.h"

KeyInput::KeyInput()
{
    data = mask;
}

KeyInput& KeyInput::operator=(u16 value)
{
    data = value & mask;
    return *this;
}

KeyInput::operator u16() const
{
    return data;
}

void KeyControl::write(uint index, u8 byte)
{
    Register::write(index, byte);

    mask = bit::seq<0, 10>(data);

    if (index == 1)
    {
        irq   = bit::seq<6, 1>(byte);
        logic = bit::seq<7, 1>(byte);
    }
}
