#include "blendbrightness.h"

#include <algorithm>

#include "common/utility.h"

void BlendBrightness::reset()
{
    evy = 0;
}

void BlendBrightness::write(u8 byte)
{
    regs.evy = bits<0, 5>(byte);
    evy = std::min(16, regs.evy);
}
