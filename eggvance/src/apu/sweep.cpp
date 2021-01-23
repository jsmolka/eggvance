#include "sweep.h"

#include "base/bit.h"

void Sweep::init()
{
    timer = period;
}

void Sweep::init(uint freq)
{
    timer   = period;
    enabled = period || shift;
    shadow  = freq;
}

bool Sweep::tick()
{
    return enabled && timer && --timer == 0;
}

void Sweep::write(u8 byte)
{
    shift  = bit::seq<0, 3>(byte);
    negate = bit::seq<3, 1>(byte);
    period = bit::seq<4, 3>(byte);

    init();
}

uint Sweep::next() const
{
    return shadow + (negate ? -1 : 1) * (shadow >> shift);
}
