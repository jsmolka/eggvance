#include "io.h"

#include "gamepad.h"

template<uint Index>
void KeyControl::write(u8 byte)
{
    Register::write<Index>(byte);

    mask = bit::seq<0, 10>(value);

    if (Index == 1)
    {
        irq  = bit::seq<6, 1>(byte);
        cond = bit::seq<7, 1>(byte);
    }

    gamepad.tryRaise();
}

template void KeyControl::write<0>(u8);
template void KeyControl::write<1>(u8);
