#include "keyinput.h"

#include "common/macros.h"

KeyInput::operator int() const
{
    return keys;
}

KeyInput& KeyInput::operator|=(int value)
{
    keys |= value;

    return *this;
}

KeyInput& KeyInput::operator&=(int value)
{
    keys &= value;

    return *this;
}

void KeyInput::reset()
{
    keys = 0x3FF;
}

u8 KeyInput::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(keys)[index];
}
