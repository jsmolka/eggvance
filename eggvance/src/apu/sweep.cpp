#include "sweep.h"

void Sweep::init()
{
    timer   = timer_init;
    enabled = isEnabled();
}

void Sweep::write(u8 byte)
{
    shift      = bit::seq<0, 3>(byte);
    negate     = bit::seq<3, 1>(byte);
    timer_init = bit::seq<4, 2>(byte);

    init();
}

bool Sweep::isEnabled() const
{
    return timer || shift;
}
