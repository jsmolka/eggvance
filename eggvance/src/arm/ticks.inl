#pragma once

#include "scheduler/scheduler.h"

SHELL_INLINE void Arm::idle(u64 cycles)
{
    pipe.access = Access::NonSequential;

    tickRam(cycles);
}

SHELL_INLINE void Arm::tickRam(u64 cycles)
{
    if (prefetch.active && !(state & State::Dma) && waitcnt.prefetch)
        prefetch.cycles += cycles;

    scheduler.run(cycles);
}

SHELL_INLINE void Arm::tickRom(u32 addr, u64 cycles)
{
    if (!(state & State::Dma) && waitcnt.prefetch)
    {
        prefetch.active = addr == pc;

        if (prefetch.active && prefetch.cycles)
        {
            u64 non = waitcnt.waitHalf(addr, Access::NonSequential);
            u64 seq = waitcnt.waitHalf(addr, Access::Sequential);

            cycles -= non - seq + std::min(8 * seq, prefetch.cycles);

            if (static_cast<s64>(cycles) <= 0)
            {
                prefetch.cycles = 0;
                return;
            }
        }
        prefetch.cycles = 0;
    }
    scheduler.run(cycles);
}

SHELL_INLINE void Arm::tickMultiply(u32 multiplier, bool sign)
{
    u64 cycles = 1;
    
    for (uint mask = 0xFFFF'FF00; mask; mask <<= 8)
    {
        multiplier &= mask;
        if (multiplier == 0 || (sign && multiplier == mask))
            break;

        cycles++;
    }
    idle(cycles);
}
