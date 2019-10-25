#include "keyinput.h"

#include "common/macros.h"
#include "common/utility.h"

void KeyInput::reset()
{
    keys = 0x3FF;
}

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

u8 KeyInput::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return bits<0, 8>(keys);
    else
        return bits<8, 2>(keys);
}

void KeyInput::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");
}
