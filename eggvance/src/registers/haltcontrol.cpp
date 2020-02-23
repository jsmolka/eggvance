#include "haltcontrol.h"

#include "arm/arm.h"

template<uint index>
inline void HaltControl::write(u8 byte)
{
    static_assert(index < 1);

    arm.state |= ARM::STATE_HALT;
}

template void HaltControl::write<0>(u8 byte);
