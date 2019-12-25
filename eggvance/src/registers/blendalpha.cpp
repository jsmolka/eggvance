#include "blendalpha.h"

#include <algorithm>

#include "common/bits.h"
#include "common/macros.h"

void BlendAlpha::reset()
{
    *this = {};
}

u8 BlendAlpha::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return data[index];
}

void BlendAlpha::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        eva = std::min(16, static_cast<int>(bits<0, 5>(byte)));
    else
        evb = std::min(16, static_cast<int>(bits<0, 5>(byte)));

    data[index] = byte;
}
