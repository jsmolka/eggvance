#include "backgroundoffset.h"

#include "common/utility.h"

void BackgroundOffset::write(int index, u8 byte)
{
    if (index == 1)
        byte &= 0x1;

    bytes(&offset)[index] = byte;
}
