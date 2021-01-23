#include "io.h"

#include "apu.h"

template<uint Index>
u8 SoundControl::read()
{
    u8 value = Register::read<Index>();

    if (Index == 0)
    {
        value |= apu.sequencer.square1.enabled;
    }
    return value;
}

template u8 SoundControl::read<0>();
template u8 SoundControl::read<1>();
