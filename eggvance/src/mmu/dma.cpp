#include "dma.h"

#include "common/format.h"
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
    count = 0;
    src.addr = 0;
    dst.addr = 0;
    control = {};

    remaining = 0;
    seq_cycles = 0;
    addr_dst = 0;
    diff_dst = 0;
    diff_src = 0;
    active = false;
}

void DMA::activate()
{
    // Todo: use actual cycles
    seq_cycles = 2;

    remaining = count > 0
        ? count
        : id < 3
            ? 0x04000
            : 0x10000;

    addr_dst = dst.addr;
    diff_dst = stepDifference(static_cast<Adjustment>(control.dst_adjust));
    diff_src = stepDifference(static_cast<Adjustment>(control.src_adjust));

    if (control.src_adjust != ADJ_RELOAD)
    {
        active = true;
    }
    else
    {
        fmt::printf("DMA: Reload for source adjustment\n");
        active = false;
    }
}

bool DMA::emulate(int& cycles)
{
    while (remaining-- > 0)
    {
        if (control.word)
            mmu.writeWord(dst.addr, mmu.readWord(src.addr));
        else
            mmu.writeHalf(dst.addr, mmu.readHalf(src.addr));

        dst.addr += diff_dst;
        src.addr += diff_src;

        cycles -= 2 + 2 * seq_cycles;
        if (cycles <= 0)
            return false;
    }

    reload();
    interrupt();

    control.enable = control.repeat;
    active = false;
    return true;
}

int DMA::stepDifference(Adjustment adj)
{
    int size = control.word ? 4 : 2;

    switch (adj)
    {
    case ADJ_INCREMENT: 
    case ADJ_RELOAD:
        return size;

    case ADJ_DECREMENT: 
        return -size;

    case ADJ_FIXED: 
        return 0; 
    }
    return 0;
}

void DMA::reload()
{
    if (control.dst_adjust == ADJ_RELOAD)
        dst.addr = addr_dst;
}

void DMA::interrupt()
{
    static constexpr InterruptFlag flags[4] = {
        IF_DMA0, IF_DMA1, IF_DMA2, IF_DMA3
    };

    if (control.irq)
        Interrupt::request(flags[id]);
}
