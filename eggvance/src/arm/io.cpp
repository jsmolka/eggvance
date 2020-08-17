#include "io.h"

#include "core/core.h"

template<uint Index>
void ArmIo::HaltControl::write(u8 byte)
{
    RegisterW<kSize, kMask>::write<Index>(byte);

    core.arm.state |= ARM::kStateHalt;
}

template void ArmIo::HaltControl::write<0>(u8);