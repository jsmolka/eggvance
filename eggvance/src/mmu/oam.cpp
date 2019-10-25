#include "oam.h"

void OAM::reset()
{
    for (auto& entry : entries)
    {
        entry.reset();
    }
    fill(0);
}

const OAMEntry& OAM::entry(int index) const
{
    return entries[index];
}

void OAM::writeHalf(u32 addr, u16 half)
{
    addr &= 0x3FE;
    int attr = addr & 0x7;
    if (attr != 0x6)
        entries[addr >> 3].writeHalf(attr, half);

    RAM::writeHalf(addr, half);
}

void OAM::writeWord(u32 addr, u32 word)
{
    addr &= 0x3FC;
    writeHalf(addr + 0, (word >>  0) & 0xFFFF);
    writeHalf(addr + 2, (word >> 16) & 0xFFFF);
}
