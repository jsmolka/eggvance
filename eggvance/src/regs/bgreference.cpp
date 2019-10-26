#include "bgreference.h"

#include "common/macros.h"
#include "common/utility.h"

void BGReference::reset()
{
    *this = {};
}

void BGReference::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 3, "Invalid index");

    if (index == 3)
        byte = signExtend<4>(byte);

    bcast(reference)[index] = byte;
    internal = reference;
}
