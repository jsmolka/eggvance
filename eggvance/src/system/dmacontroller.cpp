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
            arm.state &= ~ARM::STATE_DMA;
    }
}

void DMAController::broadcast(DMA::Timing timing)
{
    for (auto& dma : dmas)
    {
        emit(dma, timing);
    }
}

u8 DMAController::read(u32 addr) const
{    
    switch (addr)
    {
    READ_HALF_REG(REG_DMA0CNT_H, dmas[0].control);
    READ_HALF_REG(REG_DMA1CNT_H, dmas[1].control);
    READ_HALF_REG(REG_DMA2CNT_H, dmas[2].control);
    READ_HALF_REG(REG_DMA3CNT_H, dmas[3].control);

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

void DMAController::write(u32 addr, u8 byte)
{
    #define WRITE_CTRL_REG(label, dma, mask)                \
        case label + 0: writeControl<0>(dma, byte & ((mask >> 0) & 0xFF)); break;  \
        case label + 1: writeControl<1>(dma, byte & ((mask >> 8) & 0xFF)); break


    switch (addr)
    {
    WRITE_WORD_REG(REG_DMA0SAD,   dmas[0].sad, 0x7FFF'FFFF);
    WRITE_WORD_REG(REG_DMA1SAD,   dmas[1].sad, 0x0FFF'FFFF);
    WRITE_WORD_REG(REG_DMA2SAD,   dmas[2].sad, 0x0FFF'FFFF);
    WRITE_WORD_REG(REG_DMA3SAD,   dmas[3].sad, 0x0FFF'FFFF);
    WRITE_WORD_REG(REG_DMA0DAD,   dmas[0].dad, 0x7FFF'FFFF);
    WRITE_WORD_REG(REG_DMA1DAD,   dmas[1].dad, 0x7FFF'FFFF);
    WRITE_WORD_REG(REG_DMA2DAD,   dmas[2].dad, 0x7FFF'FFFF);
    WRITE_WORD_REG(REG_DMA3DAD,   dmas[3].dad, 0x0FFF'FFFF);
    WRITE_HALF_REG(REG_DMA0CNT_L, dmas[0].count, 0x0000'3FFF);
    WRITE_CTRL_REG(REG_DMA0CNT_H, dmas[0], 0x0000'F7E0);
    WRITE_HALF_REG(REG_DMA1CNT_L, dmas[1].count, 0x0000'3FFF);
    WRITE_CTRL_REG(REG_DMA1CNT_H, dmas[1], 0x0000'F7E0);
    WRITE_HALF_REG(REG_DMA2CNT_L, dmas[2].count, 0x0000'3FFF);
    WRITE_CTRL_REG(REG_DMA2CNT_H, dmas[2], 0x0000'F7E0);
    WRITE_HALF_REG(REG_DMA3CNT_L, dmas[3].count, 0x0000'FFFF);
    WRITE_CTRL_REG(REG_DMA3CNT_H, dmas[3], 0x0000'FFE0);

    default:
        EGG_UNREACHABLE;
        break;
    }

    #undef WRITE_CTRL_REG
}

void DMAController::emit(DMA& dma, DMA::Timing timing)
{
    if (!dma.running && dma.control.enable && dma.control.timing == int(timing))
    {
        dma.start();

        if (!active || dma.id < active->id)
        {
            active = &dma;
            arm.state |= ARM::STATE_DMA;
        }
    }
}

template<uint index>
void DMAController::writeControl(DMA& dma, u8 byte)
{
    dma.control.write<index>(byte);

    if (dma.control.reload)
    {
        emit(dma, DMA::Timing::Immediate);
    }
}
