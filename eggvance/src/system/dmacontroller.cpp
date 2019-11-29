#include "dmacontroller.h"

#include "common/macros.h"
#include "mmu/memmap.h"
#include "registers/macros.h"

void DMAController::reset()
{
    active = nullptr;

    for (auto& dma : dmas)
    {
        dma.reset();
    }
}

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
    }
}

void DMAController::signal(DMA::Timing timing)
{
    for (auto& dma : dmas)
    {
        const auto& control = dma.control;

        if (!dma.running && control.enabled && DMA::Timing(control.timing) == timing)
        {
            dma.start();
            if (!active || dma.id < active->id)
                active = &dma;
        }
    }
}

u8 DMAController::readByte(u32 addr)
{    
    switch (addr)
    {
    READ2(REG_DMA0CNT_H, dmas[0].control);
    READ2(REG_DMA1CNT_H, dmas[1].control);
    READ2(REG_DMA2CNT_H, dmas[2].control);
    READ2(REG_DMA3CNT_H, dmas[3].control);

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

#define WRITE_DMA_CONTROL(label, control)    \
    CASE2(label):                            \
        control.write(addr - label, byte);   \
        if (control.reload)                  \
            signal(DMA::Timing::Immediate);  \
        break

void DMAController::writeByte(u32 addr, u8 byte)
{
    switch (addr)
    {
    WRITE4(REG_DMA0SAD,   dmas[0].sad);
    WRITE4(REG_DMA1SAD,   dmas[1].sad);
    WRITE4(REG_DMA2SAD,   dmas[2].sad);
    WRITE4(REG_DMA3SAD,   dmas[3].sad);
    WRITE4(REG_DMA0DAD,   dmas[0].dad);
    WRITE4(REG_DMA1DAD,   dmas[1].dad);
    WRITE4(REG_DMA2DAD,   dmas[2].dad);
    WRITE4(REG_DMA3DAD,   dmas[3].dad);
    WRITE2(REG_DMA0CNT_L, dmas[0].count);
    WRITE2(REG_DMA1CNT_L, dmas[1].count);
    WRITE2(REG_DMA2CNT_L, dmas[2].count);
    WRITE2(REG_DMA3CNT_L, dmas[3].count);

    WRITE_DMA_CONTROL(REG_DMA0CNT_H, dmas[0].control);
    WRITE_DMA_CONTROL(REG_DMA1CNT_H, dmas[1].control);
    WRITE_DMA_CONTROL(REG_DMA2CNT_H, dmas[2].control);
    WRITE_DMA_CONTROL(REG_DMA3CNT_H, dmas[3].control);

    default:
        EGG_UNREACHABLE;
        break;
    }
}
