#include "fifo.h"

void Fifo::tick()
{
    sample = size() ? read() : 0;
}
