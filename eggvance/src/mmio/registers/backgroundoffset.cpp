#include "backgroundoffset.h"

#include "common/macros.h"
#include "common/utility.h"

void BackgroundOffset::reset()
{
    offset = 0;
}

void BackgroundOffset::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    if (index == 1)
        byte &= 0x1;

    bytes(&offset)[index] = byte;
}
