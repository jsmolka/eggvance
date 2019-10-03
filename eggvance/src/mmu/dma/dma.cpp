#include "dma.h"

#include "common/macros.h"
#include "mmu/mmu.h"
#include "mmu/interrupt.h"

#include <fmt/printf.h>

DMA::DMA(int id, MMU& mmu)
    : id(id)
    , mmu(mmu)
    , sad(mmu.mmio.dma.sad[id])
    , dad(mmu.mmio.dma.dad[id])
    , control(mmu.mmio.dma.control[id])
{
    reset();
}

bool DMA::canStart(Timing timing) const
{
    return status == DISABLED && control.enable && control.timing == timing;
}

void DMA::reset()
{
    status    = DISABLED;
    regs.sad  = 0;
    regs.dad  = 0;
    sad_delta = 0;
    dad_delta = 0;
    count     = 0;
}

void DMA::start()
{
    status    = RUNNING;
    count     = control.count;

    if (control.reload)
    {
        regs.sad       = sad.addr;
        regs.dad       = dad.addr;
        control.reload = false;
    }
    else if (control.repeat)  // Repeat DMA doesn't reload all registers
    {
        if (control.dad_control == AddressControl::RELOAD)
        {
            regs.dad = dad.addr;
        }
    }

    sad_delta = addressDelta(static_cast<AddressControl>(control.sad_control));
    dad_delta = addressDelta(static_cast<AddressControl>(control.dad_control));
}

bool DMA::transfer(int& cycles)
{
    if (control.word)
    {
        while (count-- > 0)
        {
            u32 word = mmu.readWord(regs.sad);
            mmu.writeWord(regs.dad, word);

            regs.sad += sad_delta;
            regs.dad += dad_delta;

            cycles -= 2 + 2 * sequential;
            if (cycles <= 0)
                return false;
        }
    }
    else
    {
        while (count-- > 0)
        {
            u32 half = mmu.readHalf(regs.sad);
            mmu.writeHalf(regs.dad, half);

            regs.sad += sad_delta;
            regs.dad += dad_delta;

            cycles -= 2 + 2 * sequential;
            if (cycles <= 0)
                return false;
        }
    }
    return true;
}

void DMA::finish()
{
    status = DISABLED;

    control.enable = control.repeat;

    static constexpr InterruptFlag flags[4] = {
        IF_DMA0, IF_DMA1, IF_DMA2, IF_DMA3
    };

    if (control.irq)
        Interrupt::request(flags[id]);
}

int DMA::addressDelta(AddressControl control)
{
    int size = this->control.word ? 4 : 2;

    switch (control)
    {
    case INCREMENT: return size;
    case DECREMENT: return -size;
    case FIXED:     return 0;
    case RELOAD:    return size;

    default:
        UNREACHABLE;
        break;
    }
}
