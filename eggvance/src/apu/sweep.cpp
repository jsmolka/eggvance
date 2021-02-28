#include "sweep.h"

#include "base/bit.h"

void Sweep::init()
{
    timer   = period;
    enabled = period || shift;
}

void Sweep::init(uint freq)
{
    init();

    shadow = freq;
}

bool Sweep::tick()
{
    return enabled && timer && --timer == 0;
}

void Sweep::write(u8 byte)
{
    uint was_negate = negate;

    shift  = bit::seq<0, 3>(byte);
    negate = bit::seq<3, 1>(byte);
    period = bit::seq<4, 3>(byte);

    if (was_negate && !negate)
        enabled = false;
}

uint Sweep::next() const
{
    return shadow + (negate ? -1 : 1) * (shadow >> shift);
}
