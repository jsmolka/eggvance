#include "sweep.h"

#include "base/bit.h"

void Sweep::init()
{
    timer = period;
}

void Sweep::init(uint freq)
{
    timer  = period;
    shadow = freq;
}

void Sweep::write(u8 byte)
{
    shift  = bit::seq<0, 3>(byte);
    negate = bit::seq<3, 1>(byte);
    period = bit::seq<4, 3>(byte);
    enable = period || shift;

    init();
}

uint Sweep::calculate() const
{
    return shadow + (negate ? -1 : 1) * (shadow >> shift);
}
