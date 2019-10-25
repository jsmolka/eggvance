#include "bgreference.h"

#include "common/macros.h"
#include "common/utility.h"

void BGReference::reset()
{
    *this = {};
}

u8 BGReference::readByte(int index)
{
    EGG_ASSERT(index <= 3, "Invalid index");
    return 0;
}

void BGReference::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 3, "Invalid index");

    if (index == 3)
        byte = signExtend<4>(byte);

    byteArray(reference)[index] = byte;
    internal = reference;
}
