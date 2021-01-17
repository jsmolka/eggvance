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

    enum Direction { kDec, kInc };

    if (direction == Direction::kDec)
    {
        if (volume > 0)
            volume--;
    }
    else
    {
        if (volume < 15)
            volume++;
    }
}

void Envelope::write(u8 byte)
{
    period_init = bit::seq<0, 3>(byte);
    direction   = bit::seq<3, 1>(byte);
    volume_init = bit::seq<4, 4>(byte);
}

bool Envelope::isEnabled() const
{
    return volume || direction;
}
