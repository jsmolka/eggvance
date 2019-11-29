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

    int attr = addr & 0x6;
    if (attr < 0x6)
        entries[addr >> 3].writeHalf(attr, half);

    writeHalfFast(addr, half);
}

void OAM::writeWord(u32 addr, u32 word)
{
    addr = align<u32>(addr);

    writeHalf(addr + 0, bits< 0, 16>(word));
    writeHalf(addr + 2, bits<16, 16>(word));
}

Matrix OAM::matrix(int index)
{
    return Matrix(
        readHalfFast(0x20 * index + 0x06),
        readHalfFast(0x20 * index + 0x0E),
        readHalfFast(0x20 * index + 0x16),
        readHalfFast(0x20 * index + 0x1E)
    );
}
