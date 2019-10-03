#include "dma2.h"

DMA::Status DMA2::emulate(int& cycles)
{
    if (!transfer(cycles))
        return RUNNING;

    finish();

    return DISABLED;
}
