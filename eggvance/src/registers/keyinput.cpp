#include "keyinput.h"

#include "common/macros.h"

KeyInput& KeyInput::operator=(u16 value)
{
    this->value = value;

    return *this;
}

KeyInput::operator u16() const
{
    return value;
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
