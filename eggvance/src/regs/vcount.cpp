#include "vcount.h"

#include "common/macros.h"
#include "common/utility.h"

VCount::operator int() const
{
    return line;
}

VCount& VCount::operator=(int value)
{
    line = value;
    return *this;
}

void VCount::reset()
{
    *this = {};
}

u8 VCount::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return line;
    else
        return 0;
}

void VCount::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");
}
