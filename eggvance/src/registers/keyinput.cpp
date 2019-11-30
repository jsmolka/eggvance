#include "keyinput.h"

#include "common/macros.h"

KeyInput::operator int() const
{
    return value;
}

KeyInput& KeyInput::operator|=(int value)
{
    value |= value;

    return *this;
}

KeyInput& KeyInput::operator&=(int value)
{
    value &= value;

    return *this;
}

void KeyInput::reset()
{
    value = 0x3FF;
}

u8 KeyInput::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(value)[index];
}
