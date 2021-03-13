#pragma once

#include "scheduler/scheduler.h"

SHELL_INLINE void Arm::idle(u64 cycles)
{
    pipe.access = Access::NonSequential;

    tickRam(cycles);
}

SHELL_INLINE void Arm::tickRam(u64 cycles)
{
    if (prefetch.active && !(state & kStateDma) && waitcnt.prefetch)
        prefetch.cycles += cycles;

    scheduler.run(cycles);
}

SHELL_INLINE void Arm::tickRom(u32 addr, u64 cycles)
{
    if (!(state & kStateDma) && waitcnt.prefetch)
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

template<bool Signed>
SHELL_INLINE void Arm::tickMultiply(u32 multiplier)
{
    u64 cycles = 1;

    if (Signed)
    {
        cycles += (multiplier >>  8) != 0 && (multiplier >>  8) != 0xFF'FFFF;
        cycles += (multiplier >> 16) != 0 && (multiplier >> 16) != 0x00'FFFF;
        cycles += (multiplier >> 24) != 0 && (multiplier >> 24) != 0x00'00FF;
    }
    else
    {
        cycles += (multiplier >>  8) != 0;
        cycles += (multiplier >> 16) != 0;
        cycles += (multiplier >> 24) != 0;
    }
    idle(cycles);
}
