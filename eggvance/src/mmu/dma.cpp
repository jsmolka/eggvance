#include "dma.h"

#include "mmu.h"

DMA::DMA(MMU& mmu)
    : mmu(mmu)
{
    reset();
}

void DMA::reset()
{
    units = 0;
    src.addr = 0;
    dst.addr = 0;
    control = {};
}

void DMA::run(DMATiming timing)
{
    if (!control.enable || timing != control.timing)
        return;

    u16 loop = units;

    u32 dst_addr = dst.addr;
    int addr_diff = control.word ? 4 : 2;

    int dst_delta = 0;
    switch (control.dst_control)
    {
    case DA_INC: dst_delta =  addr_diff; break;
    case DA_DEC: dst_delta = -addr_diff; break;
    case DA_FIX: dst_delta =          0; break;
    case DA_RLD: dst_delta = -addr_diff; break;
    }

    int src_delta = 0;
    switch (control.src_control)
    {
    case SA_INC: src_delta =  addr_diff; break;
    case SA_DEC: src_delta = -addr_diff; break;
    case SA_FIX: src_delta =          0; break;
    
    default:
        return;
    }

    while (loop-- > 0)
    {
        if (control.word)
            mmu.writeWord(dst.addr, mmu.readWord(src.addr));
        else
            mmu.writeHalf(dst.addr, mmu.readHalf(src.addr));

        dst.addr += dst_delta;
        src.addr += src_delta;
    }

    control.enable = control.repeat;

    if (control.dst_control == DA_RLD)
        dst.addr = dst_addr;

    // Todo: Interrupts, ...
}
