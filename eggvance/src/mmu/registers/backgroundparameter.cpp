#include "backgroundparameter.h"

#include "common/utility.h"

void BackgroundParameter::write(int index, u8 byte)
{
    bytes(&param)[index] = byte;
}
