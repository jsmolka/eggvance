#include "arm.h"

#include <cmath>

int prefetched = 0;

int Arm::prefetch(u32 addr, int wait)
{   
    if (!waitcnt.prefetch)
        return wait;

    if (addr < 0x800'0000 || addr >= 0xE00'0000)
    {
        int non = waitcnt.cyclesHalf(pc, Access::NonSequential) - 1;
        int seq = waitcnt.cyclesHalf(pc, Access::Sequential) - 1;
        int max = 8 * seq;

        prefetched = std::min(max, prefetched + wait + non - seq);
    }
    else if (prefetched > 0)
    {
        int diff = std::min(prefetched, wait);
        prefetched -= diff;
        wait -= diff;
    }
    return wait;
}