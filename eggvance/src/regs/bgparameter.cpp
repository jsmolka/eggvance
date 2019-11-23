#include "bgparameter.h"

#include "common/macros.h"

void BGParameter::reset()
{
    *this = {};
}

void BGParameter::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    bcast(value)[index] = byte;
}
