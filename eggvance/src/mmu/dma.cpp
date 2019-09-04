#include "dma.h"

#include <fmt/printf.h>

#include "interrupt.h"
#include "mmu.h"

DMA::DMA(int id, MMU& mmu)
    : id(id)
    , mmu(mmu)
    , control(mmu.mmio.dma.control[id])
    , sad(mmu.mmio.dma.sad[id])
    , dad(mmu.mmio.dma.dad[id])
{
    reset();
}

void DMA::reset()
{
    //control = {};
    sad.addr = 0;
    dad.addr = 0;

    remaining = 0;
    seq_cycles = 0;
    addr_dst = 0;
    diff_dst = 0;
    diff_src = 0;
    active = false;
}

// Todo: return active?
void DMA::activate()
{
    // Todo: use actual cycles
    seq_cycles = 2;

    remaining = control.count;

    addr_dst = dad.addr;
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
    // Todo: in activate?
    if (id == 3 && dad.addr >= 0xD00'0000 && dad.addr < 0xE00'0000)
    {
        // Guessing EEPROM size in advance seems to be pretty much impossible.
        // That's why we base the size on the first write (which should happen
        // before the first read).
        if (mmu.gamepak->save->data.empty())
        {
            switch (control.count)
            {
            // Bus width 6
            case  9:  // Set address for reading
            case 73:  // Write data to address
                mmu.gamepak->save->data.resize(0x0200, 0);
                break;

            // Bus width 14
            case 17:  // Set address for reading
            case 81:  // Write data to address
                mmu.gamepak->save->data.resize(0x2000, 0);
                break;

            default:
                fmt::printf("DMA: Unexpected EEPROM write count %d\n", control.count);
                break;
            }
        }
    }

    while (remaining-- > 0)
    {
        // Todo: check if EEPROM
        if (id == 3 && dad.addr >= 0xD00'0000 && dad.addr < 0xE00'0000)
        {
            mmu.gamepak->save->writeByte(dad.addr, (u8)mmu.readHalf(sad.addr));
        }
        else if (id == 3 && sad.addr >= 0xD00'0000 && sad.addr < 0xE00'0000)
        {
            mmu.writeHalf(dad.addr, mmu.gamepak->save->readByte(sad.addr));
        }
        else
        {
            if (control.word)
                mmu.writeWord(dad.addr, mmu.readWord(sad.addr));
            else
                mmu.writeHalf(dad.addr, mmu.readHalf(sad.addr));
        }

        dad.addr += diff_dst;
        sad.addr += diff_src;

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
        dad.addr = addr_dst;
}

void DMA::interrupt()
{
    static constexpr InterruptFlag flags[4] = {
        IF_DMA0, IF_DMA1, IF_DMA2, IF_DMA3
    };

    if (control.irq)
        Interrupt::request(flags[id]);
}
