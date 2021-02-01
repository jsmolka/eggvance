#include "fifo.h"

void Fifo::tick()
{
    sample = size() ? read() : 0;
}

bool Fifo::refillable() const
{
    return size() <= 16;
}
