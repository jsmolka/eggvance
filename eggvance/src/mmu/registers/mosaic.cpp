#include "mosaic.h"

#include "common/macros.h"
#include "common/utility.h"

void Mosaic::Stretch::reset()
{
    x = 0;
    y = 0;
}

void Mosaic::reset()
{
    bg.reset();
    obj.reset();
}

void Mosaic::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0:
        bg.x = bits<0, 4>(byte);
        bg.y = bits<4, 4>(byte);
        break;

    case 1:
        obj.x = bits<0, 4>(byte);
        obj.y = bits<4, 4>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
