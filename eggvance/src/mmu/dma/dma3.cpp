#include "dma3.h"

DMA::Status DMA3::emulate(int& cycles)
{
    if (!transfer(cycles))
        return RUNNING;

    finish();

    return DISABLED;
}
