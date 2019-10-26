#include "haltcontrol.h"

#include "common/macros.h"

HaltControl::operator bool() const
{
    return halt;
}

HaltControl& HaltControl::operator=(bool value)
{
    halt = value;

    return *this;
}

void HaltControl::reset()
{
    *this = {};
}

void HaltControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 0, "Invalid index");

    halt = true;
}
