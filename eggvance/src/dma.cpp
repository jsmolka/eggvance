#include "dma.h"

#include "arm/arm.h"
#include "mmu/mmu.h"

int DMA::deltas[4] = { 1, -1, 0, 1 };

void DMA::reset()
{
    sad.reset();
    dad.reset();
    control.reset();
}

void DMA::start()
{
    state = State::Running;
    count = control.count;

    if (control.update)
    {
        sad_addr = sad.addr;
        dad_addr = dad.addr;
        control.update = false;
    }
    else if (control.repeat && control.dad_delta == 0b11)
    {
        dad_addr = dad.addr;
    }

    sad_delta = (control.word ? 4 : 2) * deltas[control.sad_delta];
    dad_delta = (control.word ? 4 : 2) * deltas[control.dad_delta];
}

void DMA::run(int& cycles)
{
    while (count-- > 0)
    {
        if (control.word)
        {
            u32 word = mmu.readWord(sad_addr);
            mmu.writeWord(dad_addr, word);
        }
        else
        {
            u32 half = mmu.readHalf(sad_addr);
            mmu.writeHalf(dad_addr, half);
        }

        sad_addr += sad_delta;
        dad_addr += dad_delta;

        cycles -= 2 + 2 * 2;
        if (cycles <= 0)
            return;
    }

    control.enabled = control.repeat;

    if (control.irq)
    {
        static constexpr Interrupt flags[4] = {
            Interrupt::DMA0,
            Interrupt::DMA1,
            Interrupt::DMA2,
            Interrupt::DMA3
        };
        arm.request(flags[id]);
    }
    state = State::Finished;
}
