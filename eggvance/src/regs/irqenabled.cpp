#include "irqenabled.h"

#include "common/macros.h"
#include "common/utility.h"

IRQEnabled::operator int() const
{
    return value;
}

void IRQEnabled::reset()
{
    *this = {};
}

u8 IRQEnabled::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    return bytes[index];
}

void IRQEnabled::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    byteArray(value)[index] = byte;
    bytes[index] = byte;
}
