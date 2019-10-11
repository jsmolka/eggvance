#include "backgroundreference.h"

#include "common/macros.h"
#include "common/utility.h"

void BackgroundReference::reset()
{
    reference = 0;
    internal  = 0;
}

void BackgroundReference::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index >= 0 && index <= 3, "Invalid index");

    if (index == 3)
        byte = signExtend<4>(byte);

    bytes(&reference)[index] = byte;
    internal = reference;
}
