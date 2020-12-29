#include "arm.h"

#include <cmath>

struct Prefetch
{
	u32 pc = 0;
    int cycles = 0;
} prefetch;

void Arm::prefetchRam(int cycles)
{
    if (waitcnt.prefetch)
    {
        int n = waitcnt.cyclesHalf(pc, Access::NonSequential);
        int s = waitcnt.cyclesHalf(pc, Access::Sequential);

        prefetch.cycles = std::min(8 * s, prefetch.cycles + cycles + n - s);
    }
    tick(cycles);
}

void Arm::prefetchRom(u32 addr, int cycles)
{
    if (addr >= 0x800'0000 && addr < 0xE00'0000 && waitcnt.prefetch)
    {
        int diff = std::min(prefetch.cycles, cycles);
        prefetch.cycles -= diff;
        cycles -= diff;
    }
    else
    {
        prefetch.cycles = 0;
    }

    prefetch.pc = addr;

    tick(cycles);
}
