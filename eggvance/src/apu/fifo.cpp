#include "fifo.h"

void Fifo::tick()
{
    sample = static_cast<s8>(size() > 0 ? read() : 0);
}

bool Fifo::refillable() const
{
    return size() <= 16;
}
