#include "blendcontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void BlendControl::reset()
{
    *this = {};
}

u8 BlendControl::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return data[index];
}

void BlendControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        upper = bits<0, 6>(byte);
        mode  = bits<6, 2>(byte);
    }
    else
    {
        lower = bits<0, 6>(byte);
    }
    data[index] = byte;
}
