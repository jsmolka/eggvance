#include "backgroundreference.h"

#include "common/utility.h"

void BackgroundReference::write(int index, u8 byte)
{
    if (index == 3)
        byte = signExtend<4>(byte);
    bytes(&ref)[index] = byte;
    internal = ref;
}
