#include "haltcontrol.h"

#include "arm/arm.h"
#include "common/macros.h"

void HaltControl::reset()
{
    *this = {};
}

void HaltControl::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 0, "Invalid index");

    arm.state |= ARM::STATE_HALT;
}
