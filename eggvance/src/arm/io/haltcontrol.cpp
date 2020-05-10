#include "haltcontrol.h"

#include "arm/arm.h"

template<uint index>
void HaltControl::write(u8 byte)
{
    static_assert(index < 1);

    arm.state |= ARM::kStateHalt;
}

template void HaltControl::write<0>(u8 byte);
