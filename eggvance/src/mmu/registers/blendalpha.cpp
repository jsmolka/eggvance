#include "blendalpha.h"

#include <algorithm>

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
    case 0: return regs.eva;
    case 1: return regs.evb;

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
    case 0: 
        regs.eva = bits<0, 5>(byte);
        eva = std::min(16, regs.eva);
        break;

    case 1: 
        regs.evb = bits<0, 5>(byte);
        evb = std::min(16, regs.evb);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
