#include "intenabled.h"

#include "common/macros.h"

IntEnabled::operator int() const
{
    return enabled;
}

void IntEnabled::reset()
{
    *this = {};
}

u8 IntEnabled::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return bcast(enabled)[index];
}

void IntEnabled::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    bcast(enabled)[index] = byte;
}
