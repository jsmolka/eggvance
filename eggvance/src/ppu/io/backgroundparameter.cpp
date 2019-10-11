#include "backgroundparameter.h"

#include "common/macros.h"
#include "common/utility.h"

void BackgroundParameter::reset()
{
    parameter = 0;
}

void BackgroundParameter::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    bytes(&parameter)[index] = byte;
}
