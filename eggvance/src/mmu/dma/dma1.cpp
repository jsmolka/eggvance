#include "dma1.h"

DMA::Status DMA1::emulate(int& cycles)
{
    if (!transfer(cycles))
        return RUNNING;

    finish();

    return DISABLED;
}
