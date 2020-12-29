#include "arm.h"

#include <cmath>

struct Prefetch
{
    int cycles = 0;
} prefetch;

void Arm::prefetchRam(int cycles)
{
    if (waitcnt.prefetch)
    {
        int seq = waitcnt.cyclesHalf(pc, Access::Sequential);

        prefetch.cycles = std::min(8 * seq, prefetch.cycles + cycles);
    }
    tick(cycles);
}

void Arm::prefetchRom(u32 addr, int cycles)
{
    if (!waitcnt.prefetch)
        return tick(cycles);

    if (addr >= 0x800'0000 && addr < 0xE00'0000)
    {
        if (prefetch.cycles)
        {
            int non = waitcnt.cyclesHalf(addr, Access::NonSequential);
            int seq = waitcnt.cyclesHalf(addr, Access::Sequential);

            cycles -= non - seq + 1;
        }
    }

    prefetch.cycles = 0;

    tick(cycles);
}
