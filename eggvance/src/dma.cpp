#include "dma.h"

#include "arm/arm.h"
#include "mmu/mmu.h"

int DMA::deltas[4] = { 1, -1, 0, 1 };

void DMA::reset()
{
    sad.reset();
    dad.reset();
    ctrl.reset();
}

void DMA::start()
{
    state = State::Running;
    count = ctrl.count;

    if (ctrl.reload)
    {
        regs.sad = sad.addr;
        regs.dad = dad.addr;
        ctrl.reload = false;
    }
    else if (ctrl.repeat && ctrl.dad_ctrl == 0b11)
    {
        regs.dad = dad.addr;
    }

    sad_delta = (ctrl.word ? 4 : 2) * deltas[ctrl.sad_ctrl];
    dad_delta = (ctrl.word ? 4 : 2) * deltas[ctrl.dad_ctrl];
}

void DMA::run(int& cycles)
{
    if (ctrl.word)
    {
        while (count-- > 0)
        {
            u32 word = mmu.readWord(regs.sad);
            mmu.writeWord(regs.dad, word);

            regs.sad += sad_delta;
            regs.dad += dad_delta;

            cycles -= 2 + 2 * 2;
            if (cycles <= 0)
                return;
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

            cycles -= 2 + 2 * 2;
            if (cycles <= 0)
                return;
        }
    }

    state = State::Finished;

    ctrl.enable = ctrl.repeat;
    
    static constexpr Interrupt flags[4] = {
        Interrupt::DMA0,
        Interrupt::DMA1,
        Interrupt::DMA2,
        Interrupt::DMA3
    };

    if (ctrl.irq)
        arm.request(flags[id]);
}
