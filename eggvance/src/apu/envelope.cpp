#include "envelope.h"

#include "base/bit.h"

void Envelope::init()
{
    period = period_init;
    volume = volume_init;
}

void Envelope::tick()
{
    if (period && --period)
        return;

    period = period_init;

    if (volume == 0 || volume == 15)
        return;

    constexpr int kOffset[2] = { -1 , 1 };

    volume += kOffset[direction];
}

void Envelope::write(u8 byte)
{
    period_init = bit::seq<0, 3>(byte);
    direction   = bit::seq<3, 1>(byte);
    volume_init = bit::seq<4, 4>(byte);
}

bool Envelope::enable() const
{
    return volume || direction;
}
