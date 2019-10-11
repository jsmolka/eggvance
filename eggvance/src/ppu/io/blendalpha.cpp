#include "blendalpha.h"

#include "common/macros.h"
#include "common/utility.h"

void BlendAlpha::reset()
{
    eva = 0;
    evb = 0;
}

u8 BlendAlpha::readByte(int index)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0: return eva;
    case 1: return evb;

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

void BlendAlpha::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0: eva = bits<0, 5>(byte); break;
    case 1: evb = bits<0, 5>(byte); break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
