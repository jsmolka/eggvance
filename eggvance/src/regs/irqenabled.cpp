#include "irqenabled.h"

#include "common/macros.h"

IRQEnabled::operator int() const
{
    return enabled;
}

void IRQEnabled::reset()
{
    *this = {};
}

u8 IRQEnabled::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(enabled)[index];
}

void IRQEnabled::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    bcast(enabled)[index] = byte;
}
