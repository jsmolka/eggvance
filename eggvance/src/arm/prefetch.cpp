#include "arm.h"

#include <algorithm>

void Arm::tickRam(int cycles)
{
    if (waitcnt.prefetch && !(state & kStateDma) && prefetch.active)
        prefetch.cycles += cycles;

    tick(cycles);
}

void Arm::tickRom(u32 addr, int cycles)
{
    if (waitcnt.prefetch && !(state & kStateDma))
    {
        prefetch.active = addr == pc;

        if (prefetch.active && prefetch.cycles)
        {
            int non = waitcnt.cyclesHalf(addr, Access::NonSequential);
            int seq = waitcnt.cyclesHalf(addr, Access::Sequential);

            cycles -= non - seq + std::min(8 * seq, prefetch.cycles);

            if (cycles < 0)
                cycles = 0;
        }
        prefetch.cycles = 0;
    }
    tick(cycles);
}
