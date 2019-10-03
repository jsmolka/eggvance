#include "dma0.h"

DMA::Status DMA0::emulate(int& cycles)
{
    if (!transfer(cycles))
        return RUNNING;

    finish();

    return DISABLED;
}
