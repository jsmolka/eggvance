#include "arm.h"

#include <algorithm>

struct Prefetch
{
    int cycles = 0;
} prefetch;

void Arm::prefetchRam(int cycles)
{
    if (waitcnt.prefetch && !(state & kStateDma))
        prefetch.cycles += cycles;

    tick(cycles);
}

void Arm::prefetchRom(u32 addr, int cycles)
{
    if (waitcnt.prefetch && !(state & kStateDma))
    {
        if (addr == pc && prefetch.cycles > 0)
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
