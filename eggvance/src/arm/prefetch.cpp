#include "arm.h"

#include <cmath>

int prefetched = 0;

void Arm::prefetchRam(int cycles)
{
    if (!waitcnt.prefetch)
    {
        tick(cycles);
        return;
    }
    
    int non = waitcnt.cyclesHalf(pc, Access::NonSequential) - 1;
    int seq = waitcnt.cyclesHalf(pc, Access::Sequential) - 1;
    int max = 8 * seq;

    prefetched = std::min(max, prefetched + cycles + non - seq);

    tick(cycles);
}

void Arm::prefetchRom(u32 addr, int cycles)
{
    if (!waitcnt.prefetch)
    {
        tick(cycles);
        return;
    }

    int diff = std::min(prefetched, cycles);
    prefetched -= diff;
    cycles -= diff;

    tick(cycles);
}
