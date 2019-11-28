#include "oam.h"

#include "common/utility.h"

void OAM::reset()
{
    for (auto& entry : entries)
    {
        entry.reset();
    }
    fill(0);
}

void OAM::writeHalf(u32 addr, u16 half)
{
    addr = mirror(addr);
    addr = align<u16>(addr);

    int attr = addr & 0x7;
    if (attr != 0x6)
        entries[addr >> 3].writeHalf(attr, half);

    writeHalfFast(addr, half);
}

void OAM::writeWord(u32 addr, u32 word)
{
    addr = align<u32>(addr);

    writeHalf(addr + 0, bits< 0, 16>(word));
    writeHalf(addr + 2, bits<16, 16>(word));
}

s16 OAM::pa(int parameter)
{
    return readHalfFast(0x20 * parameter + 0x06);
}

s16 OAM::pb(int parameter)
{
    return readHalfFast(0x20 * parameter + 0x0E);
}

s16 OAM::pc(int parameter)
{
    return readHalfFast(0x20 * parameter + 0x16);
}

s16 OAM::pd(int parameter)
{
    return readHalfFast(0x20 * parameter + 0x1E);
}
