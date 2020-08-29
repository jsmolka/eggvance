#include "dmac.h"

#include "arm/arm.h"

void DmaController::run(int& cycles)
{
    while (active && cycles > 0)
    {
        active->run(cycles);

        if (!active->running)
        {
            active = nullptr;
            arm.state &= ~Arm::kStateDma;

            for (auto& dma : dmas)
            {
                if (dma.running)
                {
                    active = &dma;
                    arm.state |= Arm::kStateDma;
                    break;
                }
            }
        }
    }
}

void DmaController::broadcast(Dma::Timing timing)
{
    for (auto& dma : dmas)
        emit(dma, timing);
}

void DmaController::emit(Dma& dma, Dma::Timing timing)
{
    if (!dma.running
        && dma.io.control.enable
        && dma.io.control.timing == timing)
    {
        dma.activate();

        if (!active || dma.id < active->id)
        {
            active = &dma;
            arm.state |= Arm::kStateDma;
        }
    }
}
