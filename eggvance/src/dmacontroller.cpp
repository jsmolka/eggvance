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

    dmas[0].ctrl.count_mask = 0x3FFF;
    dmas[1].ctrl.count_mask = 0x3FFF;
    dmas[2].ctrl.count_mask = 0x3FFF;
    dmas[3].ctrl.count_mask = 0xFFFF;
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
        if (DMA::Timing(dma.ctrl.timing) == timing)
        {
            if (dma.ctrl.enable && dma.state != DMA::State::Running)
            {
                dma.start();
                if (!active || dma.id < active->id)
                    active = &dma;
            }
        }
    }
}

u8 DMAController::readByte(u32 addr)
{    
    switch (addr)
    {
    case REG_DMA0SAD + 0: return dmas[0].sad.readByte<0>();
    case REG_DMA0SAD + 1: return dmas[0].sad.readByte<1>();
    case REG_DMA0SAD + 2: return dmas[0].sad.readByte<2>();
    case REG_DMA0SAD + 3: return dmas[0].sad.readByte<3>();
    case REG_DMA1SAD + 0: return dmas[1].sad.readByte<0>();
    case REG_DMA1SAD + 1: return dmas[1].sad.readByte<1>();
    case REG_DMA1SAD + 2: return dmas[1].sad.readByte<2>();
    case REG_DMA1SAD + 3: return dmas[1].sad.readByte<3>();
    case REG_DMA2SAD + 0: return dmas[2].sad.readByte<0>();
    case REG_DMA2SAD + 1: return dmas[2].sad.readByte<1>();
    case REG_DMA2SAD + 2: return dmas[2].sad.readByte<2>();
    case REG_DMA2SAD + 3: return dmas[2].sad.readByte<3>();
    case REG_DMA3SAD + 0: return dmas[3].sad.readByte<0>();
    case REG_DMA3SAD + 1: return dmas[3].sad.readByte<1>();
    case REG_DMA3SAD + 2: return dmas[3].sad.readByte<2>();
    case REG_DMA3SAD + 3: return dmas[3].sad.readByte<3>();
    case REG_DMA0DAD + 0: return dmas[0].dad.readByte<0>();
    case REG_DMA0DAD + 1: return dmas[0].dad.readByte<1>();
    case REG_DMA0DAD + 2: return dmas[0].dad.readByte<2>();
    case REG_DMA0DAD + 3: return dmas[0].dad.readByte<3>();
    case REG_DMA1DAD + 0: return dmas[1].dad.readByte<0>();
    case REG_DMA1DAD + 1: return dmas[1].dad.readByte<1>();
    case REG_DMA1DAD + 2: return dmas[1].dad.readByte<2>();
    case REG_DMA1DAD + 3: return dmas[1].dad.readByte<3>();
    case REG_DMA2DAD + 0: return dmas[2].dad.readByte<0>();
    case REG_DMA2DAD + 1: return dmas[2].dad.readByte<1>();
    case REG_DMA2DAD + 2: return dmas[2].dad.readByte<2>();
    case REG_DMA2DAD + 3: return dmas[2].dad.readByte<3>();
    case REG_DMA3DAD + 0: return dmas[3].dad.readByte<0>();
    case REG_DMA3DAD + 1: return dmas[3].dad.readByte<1>();
    case REG_DMA3DAD + 2: return dmas[3].dad.readByte<2>();
    case REG_DMA3DAD + 3: return dmas[3].dad.readByte<3>();

    case REG_DMA0CNT_L + 0: return dmas[0].ctrl.readByte<0>();
    case REG_DMA0CNT_L + 1: return dmas[0].ctrl.readByte<1>();
    case REG_DMA0CNT_H + 0: return dmas[0].ctrl.readByte<2>();
    case REG_DMA0CNT_H + 1: return dmas[0].ctrl.readByte<3>();
    case REG_DMA1CNT_L + 0: return dmas[1].ctrl.readByte<0>();
    case REG_DMA1CNT_L + 1: return dmas[1].ctrl.readByte<1>();
    case REG_DMA1CNT_H + 0: return dmas[1].ctrl.readByte<2>();
    case REG_DMA1CNT_H + 1: return dmas[1].ctrl.readByte<3>();
    case REG_DMA2CNT_L + 0: return dmas[2].ctrl.readByte<0>();
    case REG_DMA2CNT_L + 1: return dmas[2].ctrl.readByte<1>();
    case REG_DMA2CNT_H + 0: return dmas[2].ctrl.readByte<2>();
    case REG_DMA2CNT_H + 1: return dmas[2].ctrl.readByte<3>();
    case REG_DMA3CNT_L + 0: return dmas[3].ctrl.readByte<0>();
    case REG_DMA3CNT_L + 1: return dmas[3].ctrl.readByte<1>();
    case REG_DMA3CNT_H + 0: return dmas[3].ctrl.readByte<2>();
    case REG_DMA3CNT_H + 1: return dmas[3].ctrl.readByte<3>();
    }
    return 0;
}

void DMAController::writeByte(u32 addr, u8 byte)
{
    switch (addr)
    {
    case REG_DMA0SAD + 0: dmas[0].sad.writeByte<0>(byte); break;
    case REG_DMA0SAD + 1: dmas[0].sad.writeByte<1>(byte); break;
    case REG_DMA0SAD + 2: dmas[0].sad.writeByte<2>(byte); break;
    case REG_DMA0SAD + 3: dmas[0].sad.writeByte<3>(byte); break;
    case REG_DMA1SAD + 0: dmas[1].sad.writeByte<0>(byte); break;
    case REG_DMA1SAD + 1: dmas[1].sad.writeByte<1>(byte); break;
    case REG_DMA1SAD + 2: dmas[1].sad.writeByte<2>(byte); break;
    case REG_DMA1SAD + 3: dmas[1].sad.writeByte<3>(byte); break;
    case REG_DMA2SAD + 0: dmas[2].sad.writeByte<0>(byte); break;
    case REG_DMA2SAD + 1: dmas[2].sad.writeByte<1>(byte); break;
    case REG_DMA2SAD + 2: dmas[2].sad.writeByte<2>(byte); break;
    case REG_DMA2SAD + 3: dmas[2].sad.writeByte<3>(byte); break;
    case REG_DMA3SAD + 0: dmas[3].sad.writeByte<0>(byte); break;
    case REG_DMA3SAD + 1: dmas[3].sad.writeByte<1>(byte); break;
    case REG_DMA3SAD + 2: dmas[3].sad.writeByte<2>(byte); break;
    case REG_DMA3SAD + 3: dmas[3].sad.writeByte<3>(byte); break;    
    case REG_DMA0DAD + 0: dmas[0].dad.writeByte<0>(byte); break;
    case REG_DMA0DAD + 1: dmas[0].dad.writeByte<1>(byte); break;
    case REG_DMA0DAD + 2: dmas[0].dad.writeByte<2>(byte); break;
    case REG_DMA0DAD + 3: dmas[0].dad.writeByte<3>(byte); break;
    case REG_DMA1DAD + 0: dmas[1].dad.writeByte<0>(byte); break;
    case REG_DMA1DAD + 1: dmas[1].dad.writeByte<1>(byte); break;
    case REG_DMA1DAD + 2: dmas[1].dad.writeByte<2>(byte); break;
    case REG_DMA1DAD + 3: dmas[1].dad.writeByte<3>(byte); break;
    case REG_DMA2DAD + 0: dmas[2].dad.writeByte<0>(byte); break;
    case REG_DMA2DAD + 1: dmas[2].dad.writeByte<1>(byte); break;
    case REG_DMA2DAD + 2: dmas[2].dad.writeByte<2>(byte); break;
    case REG_DMA2DAD + 3: dmas[2].dad.writeByte<3>(byte); break;
    case REG_DMA3DAD + 0: dmas[3].dad.writeByte<0>(byte); break;
    case REG_DMA3DAD + 1: dmas[3].dad.writeByte<1>(byte); break;
    case REG_DMA3DAD + 2: dmas[3].dad.writeByte<2>(byte); break;
    case REG_DMA3DAD + 3: dmas[3].dad.writeByte<3>(byte); break;

    case REG_DMA0CNT_L + 0: dmas[0].ctrl.writeByte<0>(byte); break;
    case REG_DMA0CNT_L + 1: dmas[0].ctrl.writeByte<1>(byte); break;
    case REG_DMA0CNT_H + 0: dmas[0].ctrl.writeByte<2>(byte); break;
    case REG_DMA0CNT_H + 1: dmas[0].ctrl.writeByte<3>(byte); signal(DMA::Timing::Immediate); break;
    case REG_DMA1CNT_L + 0: dmas[1].ctrl.writeByte<0>(byte); break;
    case REG_DMA1CNT_L + 1: dmas[1].ctrl.writeByte<1>(byte); break;
    case REG_DMA1CNT_H + 0: dmas[1].ctrl.writeByte<2>(byte); break;
    case REG_DMA1CNT_H + 1: dmas[1].ctrl.writeByte<3>(byte); signal(DMA::Timing::Immediate); break;
    case REG_DMA2CNT_L + 0: dmas[2].ctrl.writeByte<0>(byte); break;
    case REG_DMA2CNT_L + 1: dmas[2].ctrl.writeByte<1>(byte); break;
    case REG_DMA2CNT_H + 0: dmas[2].ctrl.writeByte<2>(byte); break;
    case REG_DMA2CNT_H + 1: dmas[2].ctrl.writeByte<3>(byte); signal(DMA::Timing::Immediate); break;
    case REG_DMA3CNT_L + 0: dmas[3].ctrl.writeByte<0>(byte); break;
    case REG_DMA3CNT_L + 1: dmas[3].ctrl.writeByte<1>(byte); break;
    case REG_DMA3CNT_H + 0: dmas[3].ctrl.writeByte<2>(byte); break;
    case REG_DMA3CNT_H + 1: dmas[3].ctrl.writeByte<3>(byte); signal(DMA::Timing::Immediate); break;
    }
}
