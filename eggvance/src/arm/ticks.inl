#pragma once

#include "apu/apu.h"
#include "scheduler/scheduler.h"
#include "timer/timer.h"

SHELL_INLINE void Arm::tick(int cycles)
{
    this->cycles -= cycles;

    scheduler.run(cycles);
}

SHELL_INLINE void Arm::idle(int cycles)
{
    pipe.access = Access::NonSequential;

    tickRam(cycles);
}

SHELL_INLINE void Arm::tickRam(int cycles)
{
    if (waitcnt.prefetch && !(state & kStateDma) && prefetch.active)
        prefetch.cycles += cycles;

    tick(cycles);
}

SHELL_INLINE void Arm::tickRom(u32 addr, int cycles)
{
    if (waitcnt.prefetch && !(state & kStateDma))
    {
        prefetch.active = addr == pc;

        if (prefetch.active && prefetch.cycles)
        {
            int non = waitcnt.cyclesHalf(addr, Access::NonSequential);
            int seq = waitcnt.cyclesHalf(addr, Access::Sequential);

            cycles -= non - seq + std::min(8 * seq, prefetch.cycles);

            if (cycles <= 0)
            {
                prefetch.cycles = 0;
                return;
            }
        }
        prefetch.cycles = 0;
    }
    tick(cycles);
}

template<bool Signed>
SHELL_INLINE void Arm::tickMultiply(u32 multiplier)
{
    int cycles = 1;

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
