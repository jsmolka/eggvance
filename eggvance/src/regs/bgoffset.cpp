#include "bgoffset.h"

#include "common/macros.h"

void BGOffset::reset()
{
    *this = {};
}

void BGOffset::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    bcast(offset)[index] = byte;
}
