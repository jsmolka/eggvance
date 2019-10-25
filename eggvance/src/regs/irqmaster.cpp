#include "irqmaster.h"

#include "common/macros.h"
#include "common/utility.h"

IRQMaster::operator bool() const
{
    return bytes[0] & 0x1;
}

void IRQMaster::reset()
{
    *this = {};
}

u8 IRQMaster::readByte(int index)
{
    EGG_ASSERT(index <= 3, "Invalid index");
    return bytes[index];
}

void IRQMaster::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 3, "Invalid index");
    bytes[index] = byte;
}
