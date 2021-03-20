#include "io.h"

#include "keypad.h"

KeyInput::KeyInput()
{
    value = mask;
}

KeyInput& KeyInput::operator=(u16 value)
{
    this->value = value & mask;

    return *this;
}

KeyInput::operator u16() const
{
    return value;
}

void KeyControl::write(uint index, u8 byte)
{
    Register::write(index, byte);

    mask = bit::seq<0, 10>(value);

    if (index == 1)
    {
        irq  = bit::seq<6, 1>(byte);
        cond = bit::seq<7, 1>(byte);
    }

    keypad.checkInterrupt();
}
