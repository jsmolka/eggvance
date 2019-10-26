#include "keyinput.h"

#include "common/macros.h"

KeyInput::operator int() const
{
    return input;
}

KeyInput& KeyInput::operator|=(int value)
{
    input |= value;

    return *this;
}

KeyInput& KeyInput::operator&=(int value)
{
    input &= value;

    return *this;
}

void KeyInput::reset()
{
    input = 0x3FF;
}

u8 KeyInput::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(input)[index];
}
