#include "vcount.h"

#include "common/macros.h"

VCount::operator int() const
{
    return data[0];
}

VCount& VCount::operator=(int value)
{
    data[0] = value;

    return *this;
}

void VCount::reset()
{
    *this = {};
}

u8 VCount::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return data[index];
}
