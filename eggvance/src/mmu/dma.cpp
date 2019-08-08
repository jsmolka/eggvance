#include "dma.h"

#include "interrupt.h"
#include "mmu.h"

DMA::DMA(int id, MMU& mmu)
    : id(id)
    , mmu(mmu)
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

    int loop = units;
    if (loop == 0)
    {
        loop = id < 3
            ? 0x04000
            : 0x10000;
    }

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

    static constexpr InterruptFlag flags[4] = {
        IF_DMA0, IF_DMA1, IF_DMA2, IF_DMA3
    };

    if (control.irq)
        Interrupt::request(flags[id]);
}
