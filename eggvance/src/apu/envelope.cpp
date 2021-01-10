#include "envelope.h"

void Envelope::tick()
{
    if (volume == 0 || volume == 15)
        return;

    constexpr int kOffset[2] = { -1 , 1 };

    volume += kOffset[direction];
}

void Envelope::write(u8 byte)
{
    step_time = bit::seq<0, 3>(byte);
    direction = bit::seq<3, 1>(byte);
    initial   = bit::seq<4, 4>(byte);
}
