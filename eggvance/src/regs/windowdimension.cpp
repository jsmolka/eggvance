#include "windowdimension.h"

#include "common/macros.h"

WindowDimension::WindowDimension(int limit)
{
    this->limit = limit;
}

void WindowDimension::reset()
{
    *this = { limit };
}

void WindowDimension::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    data[index] = byte;

    max = data[0];
    min = data[1];

    if (max > limit || max < min)
        max = limit;
}

bool WindowDimension::contains(int x) const
{
    return x >= min && x < max;
}
