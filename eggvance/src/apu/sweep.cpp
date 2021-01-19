#include "sweep.h"

void Sweep::init()
{
    timer = period;
}

void Sweep::init(uint frequency)
{
    timer  = period;
    shadow = frequency;
}

u8 Sweep::read() const
{
    return 0
        | shift  << 0
        | negate << 3
        | period << 4;
}

void Sweep::write(u8 byte)
{
    shift  = bit::seq<0, 3>(byte);
    negate = bit::seq<3, 1>(byte);
    period = bit::seq<4, 3>(byte);
    enable = period || shift;

    init();
}
