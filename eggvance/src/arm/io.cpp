#include "io.h"

#include "arm/arm.h"

template<uint Index>
void HaltControl::write(u8 byte)
{
    RegisterW::write<Index>(byte);

    arm.state |= kStateHalt;
}

template void HaltControl::write<0>(u8);
