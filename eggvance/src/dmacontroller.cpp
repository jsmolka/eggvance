#include "dmacontroller.h"

#include "mmu/memmap.h"

DMAController::DMAController()
{
    dmas[0].id = 0;
    dmas[1].id = 1;
    dmas[2].id = 2;
    dmas[3].id = 3;

    dmas[0].sad.addr_mask = 0x7FF'FFFF;
    dmas[1].sad.addr_mask = 0xFFF'FFFF;
    dmas[2].sad.addr_mask = 0xFFF'FFFF;
    dmas[3].sad.addr_mask = 0xFFF'FFFF;

    dmas[0].dad.addr_mask = 0xFFF'FFFF;
    dmas[1].dad.addr_mask = 0x7FF'FFFF;
    dmas[2].dad.addr_mask = 0x7FF'FFFF;
    dmas[3].dad.addr_mask = 0x7FF'FFFF;

    dmas[0].control.count_mask = 0x3FFF;
    dmas[1].control.count_mask = 0x3FFF;
    dmas[2].control.count_mask = 0x3FFF;
    dmas[3].control.count_mask = 0xFFFF;
}

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

    if (active->state == DMA::State::Finished)
    {
        active = nullptr;

        for (auto& dma : dmas)
        {
            if (dma.state == DMA::State::Running)
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
        if (DMA::Timing(dma.control.timing) == timing)
        {
            if (dma.control.enabled && dma.state != DMA::State::Running)
            {
                dma.start();
                if (!active || dma.id < active->id)
                    active = &dma;
            }
        }
    }
}

#define READ(label, reg)                       \
    case label + 0: return reg.readByte<0>();  \
    case label + 1: return reg.readByte<1>();  \
    case label + 2: return reg.readByte<2>();  \
    case label + 3: return reg.readByte<3>()

u8 DMAController::readByte(u32 addr)
{    
    switch (addr)
    {
    READ(REG_DMA0SAD, dmas[0].sad);
    READ(REG_DMA1SAD, dmas[1].sad);
    READ(REG_DMA2SAD, dmas[2].sad);
    READ(REG_DMA3SAD, dmas[3].sad);
    READ(REG_DMA0DAD, dmas[0].dad);
    READ(REG_DMA1DAD, dmas[1].dad);
    READ(REG_DMA2DAD, dmas[2].dad);
    READ(REG_DMA3DAD, dmas[3].dad);
    READ(REG_DMA0CNT, dmas[0].control);
    READ(REG_DMA1CNT, dmas[1].control);
    READ(REG_DMA2CNT, dmas[2].control);
    READ(REG_DMA3CNT, dmas[3].control);
    }
    return 0;
}

#undef READ

#define WRITE(label, reg)                           \
    case label + 0: reg.writeByte<0>(byte); break;  \
    case label + 1: reg.writeByte<1>(byte); break;  \
    case label + 2: reg.writeByte<2>(byte); break;  \
    case label + 3: reg.writeByte<3>(byte); break

void DMAController::writeByte(u32 addr, u8 byte)
{
    switch (addr)
    {
    WRITE(REG_DMA0SAD, dmas[0].sad);
    WRITE(REG_DMA1SAD, dmas[1].sad);
    WRITE(REG_DMA2SAD, dmas[2].sad);
    WRITE(REG_DMA3SAD, dmas[3].sad);
    WRITE(REG_DMA0DAD, dmas[0].dad);
    WRITE(REG_DMA1DAD, dmas[1].dad);
    WRITE(REG_DMA2DAD, dmas[2].dad);
    WRITE(REG_DMA3DAD, dmas[3].dad);
    WRITE(REG_DMA0CNT, dmas[0].control);
    WRITE(REG_DMA1CNT, dmas[1].control);
    WRITE(REG_DMA2CNT, dmas[2].control);
    WRITE(REG_DMA3CNT, dmas[3].control);
    }

    switch (addr)
    {
    case REG_DMA0CNT + 3:
    case REG_DMA1CNT + 3:
    case REG_DMA2CNT + 3:
    case REG_DMA3CNT + 3:
        signal(DMA::Timing::Immediate);
        break;
    }
}

#undef WRITE
