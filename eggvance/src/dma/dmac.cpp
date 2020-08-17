#include "dmac.h"

#include "core/core.h"

DmaController::DmaController(Core& core)
    : core(core)
    , dmas{ { core, 1 }, { core, 2 }, { core, 3 }, { core, 4} }
{

}

void DmaController::run(int& cycles)
{
    while (active && cycles > 0)
    {
        active->run(cycles);

        if (!active->running)
        {
            active = nullptr;
            core.arm.state &= ~ARM::kStateDma;

            for (auto& dma : dmas)
            {
                if (dma.running)
                {
                    active = &dma;
                    core.arm.state |= ARM::kStateDma;
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
            core.arm.state |= ARM::kStateDma;
        }
    }
}
