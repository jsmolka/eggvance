#include "dmaaddr.h"

void DMAAddr::reset()
{
    addr = 0;
}

void DMAAddr::update()
{
    addr &= addr_mask;
}
