#include "envelope.h"

#include "base/bit.h"

void Envelope::init()
{
    timer  = period;
    volume = initial;
}

void Envelope::tick()
{
    if (timer && --timer)
        return;

    if (increase)
    {
        if (volume < 15)
            volume++;
    }
    else
    {
        if (volume > 0)
            volume--;
    }

    timer = period;
}

void Envelope::write(u8 byte)
{
    period   = bit::seq<0, 3>(byte);
    increase = bit::seq<3, 1>(byte);
    initial  = bit::seq<4, 4>(byte);
}

bool Envelope::isEnabled() const
{
    return volume || increase;
}
