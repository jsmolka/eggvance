#include "mosaic.h"

#include "common/macros.h"
#include "common/utility.h"

void Mosaic::reset()
{
    bgs.x = 1;
    bgs.y = 1;
    obj.x = 1;
    obj.y = 1;
}

void Mosaic::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        bgs.x = bits<0, 4>(byte) + 1;
        bgs.y = bits<4, 4>(byte) + 1;
    }
    else
    {
        obj.x = bits<0, 4>(byte) + 1;
        obj.y = bits<4, 4>(byte) + 1;
    }
}
