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
        if (id == 3 && dst.addr >= 0xD00'0000 && dst.addr < 0xE00'0000)
        {
            //fmt::printf("W %X\n", mmu.readHalf(src.addr) & 0x1);

            mmu.gamepak->save->writeByte(dst.addr, (u8)mmu.readHalf(src.addr));
        }
        else if (id == 3 && src.addr >= 0xD00'0000 && src.addr < 0xE00'0000)
        {
            //fmt::printf("R\n");

            mmu.writeHalf(dst.addr, mmu.gamepak->save->readByte(src.addr));
        }
        else
        {
            if (control.word)
                mmu.writeWord(dst.addr, mmu.readWord(src.addr));
            else
                mmu.writeHalf(dst.addr, mmu.readHalf(src.addr));
        }

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
    writeback();

    return true;
}

void DMA::writeback()
{
    u16 value = 0;
    value |= control.dst_adjust  <<  5;
    value |= control.src_adjust  <<  7;
    value |= control.repeat      <<  9;
    value |= control.word        << 10;
    value |= control.gamepak_drq << 11;
    value |= control.timing      << 12;
    value |= control.irq         << 14;
    value |= control.enable      << 15;

    switch (id)
    {
    case 0: mmu.io.ref<u16>(REG_DMA0CNT_H) = value; break;
    case 1: mmu.io.ref<u16>(REG_DMA1CNT_H) = value; break;
    case 2: mmu.io.ref<u16>(REG_DMA2CNT_H) = value; break;
    case 3: mmu.io.ref<u16>(REG_DMA3CNT_H) = value; break;
    }
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
