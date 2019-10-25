#include "bgparameter.h"

#include "common/macros.h"
#include "common/utility.h"

void BGParameter::reset()
{
    *this = {};
}

u8 BGParameter::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    return 0;
}

void BGParameter::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    byteArray(parameter)[index] = byte;
}
