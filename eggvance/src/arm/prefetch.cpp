#include "arm.h"

#include <cmath>

struct Prefetch
{
    int cycles = 0;
} prefetch;

void Arm::prefetchRam(int cycles)
{
    if (waitcnt.prefetch)
        prefetch.cycles += cycles;

    tick(cycles);
}

void Arm::prefetchRom(u32 addr, int cycles)
{
    if (!waitcnt.prefetch)
        return tick(cycles);

    if (addr >= 0x800'0000 && addr < 0xE00'0000 && prefetch.cycles > 0)
    {
        int non = waitcnt.cyclesHalf(addr, Access::NonSequential);
        int seq = waitcnt.cyclesHalf(addr, Access::Sequential);

        cycles -= non - seq + std::min(8 * seq, prefetch.cycles);
    }

    prefetch.cycles = 0;

    tick(cycles);
}
