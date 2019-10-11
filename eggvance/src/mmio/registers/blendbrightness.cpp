#include "blendbrightness.h"

#include "common/utility.h"

void BlendBrightness::reset()
{
    evy = 0;
}

void BlendBrightness::write(u8 byte)
{
    evy = bits<0, 5>(byte);
}
