#include "dmac.h"

#include "arm/arm.h"

DMAController dmac;

void DMAController::run(int& cycles)
{
    active->run(cycles);

    if (!active->running)
    {
        active = nullptr;

        for (auto& dma : dmas)
        {
            if (dma.running)
            {
                active = &dma;
                break;
            }
        }
        
        if (!active)
            arm.state &= ~ARM::kStateDma;
    }
}

void DMAController::broadcast(DMA::Timing timing)
{
    for (auto& dma : dmas)
    {
        emit(dma, timing);
    }
}

void DMAController::emit(DMA& dma, DMA::Timing timing)
{
    if (!dma.running && dma.io.control.enable && dma.io.control.timing == int(timing))
    {
        dma.start();

        if (!active || dma.id < active->id)
        {
            active = &dma;
            arm.state |= ARM::kStateDma;
        }
    }
}
