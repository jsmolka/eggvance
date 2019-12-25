#include "intmaster.h"

#include "common/bits.h"
#include "common/macros.h"

IntMaster::operator bool() const
{
    return master;
}

void IntMaster::reset()
{
    *this = {};
}

u8 IntMaster::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(master)[index];
}

void IntMaster::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    bcast(master)[index] = byte;
}
