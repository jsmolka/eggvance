#include "dmacontroller.h"

#include "arm/arm.h"
#include "common/macros.h"
#include "mmu/memmap.h"
#include "registers/macros.h"

DMAController dmac;

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
        
        if (!active)
            arm.updateDispatch();
    }
}

void DMAController::broadcast(DMA::Timing timing)
{
    for (auto& dma : dmas)
    {
        emit(dma, timing);
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

#define WRITE_DMA_CONTROL(label, dma)           \
    CASE2(label):                               \
        writeControl(dma, addr - label, byte);  \
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

    WRITE_DMA_CONTROL(REG_DMA0CNT_H, dmas[0]);
    WRITE_DMA_CONTROL(REG_DMA1CNT_H, dmas[1]);
    WRITE_DMA_CONTROL(REG_DMA2CNT_H, dmas[2]);
    WRITE_DMA_CONTROL(REG_DMA3CNT_H, dmas[3]);

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void DMAController::emit(DMA& dma, DMA::Timing timing)
{
    if (!dma.running && dma.control.enabled && dma.control.timing == int(timing))
    {
        dma.start();

        if (!active || dma.id < active->id)
        {
            active = &dma;
            arm.updateDispatch();
        }
    }
}

void DMAController::writeControl(DMA& dma, int index, u8 byte)
{
    dma.control.write(index, byte);

    if (dma.control.reload)
    {
        emit(dma, DMA::Timing::Immediate);
    }
}
