#include "mosaic.h"

#include "common/macros.h"
#include "common/utility.h"

void Mosaic::reset()
{
    *this = {};
}

u8 Mosaic::readByte(int index)
{
    EGG_ASSERT(index <= 3, "Invalid index");
    return 0;
}

void Mosaic::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 3, "Invalid index");

    switch (index)
    {
    case 0:
        bgs.x = bits<0, 4>(byte) + 1;
        bgs.y = bits<4, 4>(byte) + 1;
        break;

    case 1:
        obj.x = bits<0, 4>(byte) + 1;
        obj.y = bits<4, 4>(byte) + 1;
        break;

    case 2:
    case 3:
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
