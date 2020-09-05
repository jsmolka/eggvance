#include "io.h"

#include "arm/arm.h"

template<uint Index>
void HaltControl::write(u8 byte)
{
    RegisterW<kSize, kMask>::write<Index>(byte);

    arm.state |= Arm::kStateHalt;
}

template void HaltControl::write<0>(u8);
