#include "blendalpha.h"

#include <algorithm>

#include "common/macros.h"
#include "common/utility.h"

void BlendAlpha::reset()
{
    *this = {};
}

u8 BlendAlpha::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    return bytes[index];
}

void BlendAlpha::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        eva = std::min(16, bits<0, 5>(byte));
    else
        evb = std::min(16, bits<0, 5>(byte));

    bytes[index] = byte;
}
