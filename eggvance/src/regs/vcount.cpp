#include "vcount.h"

#include "common/macros.h"

VCount::operator int() const
{
    return data[0];
}

void VCount::reset()
{
    *this = {};
}

u8 VCount::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return data[index];
}

void VCount::next()
{
    data[0] = (data[0] + 1) % 228;
}
