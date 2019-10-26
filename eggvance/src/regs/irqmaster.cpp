#include "irqmaster.h"

#include "common/macros.h"
#include "common/utility.h"

IRQMaster::operator bool() const
{
    return master;
}

void IRQMaster::reset()
{
    *this = {};
}

u8 IRQMaster::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(master)[index];
}

void IRQMaster::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    bcast(master)[index] = byte;
}
